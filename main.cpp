#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <fstream>
#include "lib/debug.h"
#include "lib/extractPuzzle.h"
#include "opencv2/text.hpp"

using namespace cv::text;

using namespace cv;
using namespace cv::ml;
using namespace std;

Scalar white(255, 255, 255);
Scalar green(0, 235, 0);
Scalar red(255, 0, 0);
Scalar blue(0, 0, 255);

Scalar randomColor(RNG &rng) {
    int icolor = (unsigned) rng;
    return Scalar(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);
}

Ptr<ANN_MLP> mlp;
const int hiddenLayerSize = 3;


const char strCharacters[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
const int numCharacters = 9;
const int numFilesChars[] = {11, 14, 15, 13, 13, 18, 18, 15, 9};

Mat getCell(Mat sudoku, int numCell) {
    Mat output = sudoku.clone();
    int y = sudoku.cols;
    int x = sudoku.rows;
    int cx = x / 9;
    int cy = y / 9;

//    cout << "cell size is :" << cx << " * " << cy << " = " << cx * cy << endl;

    Rect rect = Rect((numCell % 9) * cy, (numCell / 9) * cx, cy, cx);
    return output(rect);
}


Mat removeLight(Mat img, Mat pattern, int method) {
    Mat aux;
// if method is normalization
    if (method == 1) {
// Require change our image to 32 float for division
        Mat img32, pattern32;
        img.convertTo(img32, CV_32F);
        pattern.convertTo(pattern32, CV_32F);
// Divide the image by the pattern
        aux = 1 - (img32 / pattern32);
        // Scale it to convert to 8bit format
        aux = aux * 255;
// Convert 8 bits format
        aux.convertTo(aux, CV_8U);
    } else {
        aux = pattern - img;
    }
    return aux;
}

Mat calculateLightPattern(Mat img) {
    Mat pattern;
// Basic and effective way to calculate the light pattern from one image
    blur(img, pattern, Size(img.cols / 3, img.cols / 3));
//    showImage(pattern);
    return pattern;
}


Mat normalizeSize(Mat in) {

    int charSize = 25;
//Remap image
    int h = in.rows;
    int w = in.cols;
    Mat transformMat = Mat::eye(2, 3, CV_32F);
    int m = max(w, h);
    transformMat.at<float>(0, 2) = m / 2 - w / 2;
    transformMat.at<float>(1, 2) = m / 2 - h / 2;

    Mat warpImage(m, m, in.type());
    warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));

    Mat out;
    resize(warpImage, out, Size(charSize, charSize));

    return out;
}

Mat extractRoiFromCell(Mat cell) {

    int cell_height = cell.rows;
    int cell_width = cell.cols;

    // setting parameters for long lines filtering
    float percent = 0.23;
    float width_threshold = cell_width - cell_width * percent;
    float height_threshold = cell_height - cell_height * percent;

    // Use connected components with stats
    Mat labels, stats, centroids;
    int num_objects = connectedComponentsWithStats(cell, labels, stats, centroids);
    // Check the number of objects detected
    Mat output;

    if (num_objects < 2) {
//        cout << "No objects detected" << endl;
        return output;
    } else {
//        cout << "Number of objects detected: " << num_objects - 1 << endl;
    }

    // Create output image coloring the objects and show area
    for (int i = 1; i < num_objects; i++) {
        int area = stats.at<int>(i, CC_STAT_AREA);
        int width = stats.at<int>(i, CC_STAT_WIDTH);
        int height = stats.at<int>(i, CC_STAT_HEIGHT);
        int left = stats.at<int>(i, CC_STAT_LEFT);
        int top = stats.at<int>(i, CC_STAT_TOP);

        // filtering
        int boundingArea = width * height;
        if (width > width_threshold) continue; // horizontal line
        if (height > height_threshold) continue; // vetical line
        if (boundingArea < 220 || boundingArea > 900) continue;
        if (area < 110) continue; // area of the connected object

        // should be the number here
        Rect rect(left, top, width, height);
//        Mat mask= labels==i;
//        output.setTo(randomColor(rng), mask);
        return normalizeSize(cell(rect));


    }

    return output;

}

char *identifyText(Mat input, char *language = "eng") {
    tesseract::TessBaseAPI ocr;
    ocr.Init("./tessdata", language, tesseract::OEM_TESSERACT_ONLY);
    ocr.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    ocr.SetImage(input.data, input.cols, input.rows, 1,
                 input.step);
    ocr.SetVariable("tessedit_char_whitelist", "0123456789");

    char *text = ocr.GetUTF8Text();
//    cout << "Text:" << endl;
//    cout << text << endl;
//    cout << "Confidence: " << ocr.MeanTextConf() << endl << endl;
// Get the text

    return text;
}

/**
 * pre training
 *
 * ├── data
│*   ├── 1
│*   ├── 2
│*   ├── 3
│*   ├── 4
│*   ├── 5
│*   ├── 6
│*   ├── 7
│*   ├── 8
│*   └── 9
 *
 * create file image
 * An input representing a 4 will be saved in 4 if recognize if not in the parent folder
 *
 * */
void createImagesForTraining(Mat input, int fileNumber, int cellNumber) {

    string number = identifyText(input);

    std::stringstream trimmer;
    trimmer << number;
    number.clear();
    trimmer >> number;

    cout << "number is:" << number << endl;

    std::stringstream ss;
    if (number == "") {
        ss << "./data/s" << fileNumber << "-" << cellNumber << ".jpg";
    } else {
        ss << "./data/" << number << "/s" << fileNumber << "-" << cellNumber << ".jpg";
    }

    std::string filename = ss.str();
    cout << filename << endl;

    imwrite(filename, input);
}


Mat ProjectedHistogram(Mat img, int t) {
    int sz = (t) ? img.rows : img.cols;
    Mat mhist = Mat::zeros(1, sz, CV_32F);
    for (int j = 0; j < sz; j++) {
        Mat data = (t) ? img.row(j) : img.col(j);
        mhist.at<float>(j) = countNonZero(data);
    }
    //Normalize histogram
    double min, max;
    minMaxLoc(mhist, &min, &max);
    if (max > 0)
        mhist.convertTo(mhist, -1, 1.0f / max, 0);
    return mhist;
}


Mat features(Mat in, int sizeData) {
    int HORIZONTAL = 1;
    int VERTICAL = 0;

    //Histogram features
    Mat vhist = ProjectedHistogram(in, VERTICAL);
    Mat hhist = ProjectedHistogram(in, HORIZONTAL);

    //Low data feature
    Mat lowData;
    resize(in, lowData, Size(sizeData, sizeData));

    //Last 10 is the number of moments components
    int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;

    Mat out = Mat::zeros(1, numCols, CV_32F);
    //Asign values to feature
    int j = 0;
    for (int i = 0; i < vhist.cols; i++) {
        out.at<float>(j) = vhist.at<float>(i);
        j++;
    }
    for (int i = 0; i < hhist.cols; i++) {
        out.at<float>(j) = hhist.at<float>(i);
        j++;
    }
    for (int x = 0; x < lowData.cols; x++) {
        for (int y = 0; y < lowData.rows; y++) {
            out.at<float>(j) = (float) lowData.at < unsigned
            char > (x, y);
            j++;
        }
    }
    return out;
}

void createDataForTraining() {

    char *path = "./training/";

    Mat classes;
    Mat trainingDataf5;
    Mat trainingDataf10;
    Mat trainingDataf15;
    Mat trainingDataf20;

    vector<int> trainingLabels;
    for (int i = 0; i < numCharacters; i++) {
        int numFiles = numFilesChars[i];
        for (int j = 1; j <= numFiles; j++) {

            stringstream ss(stringstream::in | stringstream::out);
            ss << path << strCharacters[i] << "/" << j << ".jpg";

            Mat img = imread(ss.str(), 0);
            Mat f5 = features(img, 5);
            Mat f10 = features(img, 10);
            Mat f15 = features(img, 15);
            Mat f20 = features(img, 20);

            trainingDataf5.push_back(f5);
            trainingDataf10.push_back(f10);
            trainingDataf15.push_back(f15);
            trainingDataf20.push_back(f20);
            trainingLabels.push_back(i);
        }
    }

    trainingDataf5.convertTo(trainingDataf5, CV_32F);
    trainingDataf10.convertTo(trainingDataf10, CV_32F);
    trainingDataf15.convertTo(trainingDataf15, CV_32F);
    trainingDataf20.convertTo(trainingDataf20, CV_32F);
    Mat(trainingLabels).copyTo(classes);

    cout << classes << endl;

    FileStorage fs("OCR.xml", FileStorage::WRITE);
    fs << "TrainingDataF5" << trainingDataf5;
    fs << "TrainingDataF10" << trainingDataf10;
    fs << "TrainingDataF15" << trainingDataf15;
    fs << "TrainingDataF20" << trainingDataf20;
    fs << "classes" << classes;
    fs.release();

}


int classify(Mat f) {
    int result = -1;
    Mat output(1, numCharacters, CV_32FC1);
    mlp->predict(f, output);
    Point maxLoc;
    double maxVal;
    minMaxLoc(output, 0, &maxVal, 0, &maxLoc);
//We need to know where in output is the max val, the x (cols) is
//the class.
    return maxLoc.x;
}

TermCriteria TC(int iters, double eps)
{
    return TermCriteria(TermCriteria::MAX_ITER + (eps > 0 ? TermCriteria::EPS : 0), iters, eps);
}

static void test_and_save_classifier(const Ptr<StatModel>& model,
                                     const Mat& data, const Mat& responses,
                                     int ntrain_samples, int rdelta,
                                     const string& filename_to_save)
{
    int i, nsamples_all = data.rows;
    double train_hr = 0, test_hr = 0;

    // compute prediction error on train and test data
    for( i = 0; i < nsamples_all; i++ )
    {
        Mat sample = data.row(i);

        float r = model->predict( sample );
        r = std::abs(r + rdelta - responses.at<int>(i)) <= FLT_EPSILON ? 1.f : 0.f;

        if( i < ntrain_samples )
            train_hr += r;
        else
            test_hr += r;
    }

    test_hr /= nsamples_all - ntrain_samples;
    train_hr = ntrain_samples > 0 ? train_hr/ntrain_samples : 1.;

    printf( "Recognition rate: train = %.1f%%, test = %.1f%%\n",
            train_hr*100., test_hr*100. );

    if( !filename_to_save.empty() )
    {
        model->save( filename_to_save );
    }
}

int main(int argc, char **argv) {

    const char *files[] = {
            "../puzzles/s0.jpg",
            "../puzzles/s1.jpg",
            "../puzzles/s2.jpg",
            "../puzzles/s3.jpg",
    };

    unsigned nb_files = sizeof(files) / sizeof(const char *);

    for (unsigned j = 0; j < nb_files; ++j) {
        int error = 0;
        Mat raw = imread(files[j], CV_LOAD_IMAGE_GRAYSCALE);
        Mat preprocessed = preprocess(raw.clone());
        vector<Point> biggestApprox = findBigestApprox(preprocessed);

        Mat sudoku = extractPuzzle(raw, biggestApprox);

        showImage(sudoku);

    }

    return 0;
}
