#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <fstream>
#include "lib/debug.h"
#include "lib/extractPuzzle.h"
#include "lib/cellHandling.h"
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

        for (unsigned i = 0; i < 81; i++) {
            Mat cell = getCell(sudoku, i), cell_no_noise, cell_no_light, final_cell;

            // remove noise
            medianBlur(cell, cell_no_noise, 1);
            // remove background/light
            cell_no_light = removeLight(cell_no_noise, calculateLightPattern(cell), 2);
            // binarize image
            adaptiveThreshold(cell_no_light, final_cell, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);

            Mat roi = extractRoiFromCell(final_cell);




            if (roi.empty()) {

            } else {
                showImage(roi);
            }




        }

    }

    return 0;
}
