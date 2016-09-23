#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "debug.h"

using namespace cv;
using namespace std;

Scalar white(255, 255, 255);
Scalar green(0, 235, 0);
Scalar red(255, 0, 0);
Scalar blue(0, 0, 255);

Scalar randomColor( RNG& rng )
{
    int icolor = (unsigned) rng;
    return Scalar( icolor&255, (icolor>>8)&255, (icolor>>16)&255 );
}

/*
 * preprocess
 *
 * It's going to be easier for the opencv algorithm to detect the border of the sudoku
 * There is a Threshold where everything is white or black
 */
Mat preprocess(Mat input) {
    Mat outerBox = Mat(input.size(), CV_8UC1);
    GaussianBlur(input, input, Size(11, 11), 0);
    adaptiveThreshold(input, outerBox, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);
    bitwise_not(outerBox, outerBox);
    dilate(outerBox, outerBox, Mat());
    return outerBox;
}

/*
 * findBigestApprox
 *
 * Find the biggest contour in the image
 * note that it returns vector< vector<Point> > because it is more convenient to use drawContours after
 * */
vector<Point> findBigestApprox(Mat input) {

    int largest_area = 0;
    vector<vector<Point> > contours;
    vector<cv::Point> approx;
    vector<cv::Point> biggestApprox;

    findContours(input, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) {
        // Approximate contour with accuracy proportional to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.1, true);
        // Skip small or non-convex objects
        if (std::fabs(contourArea(contours[i])) < 1200 || !isContourConvex(approx))
            continue;

        if (approx.size() == 4) {
            double a = contourArea(contours[i]);
            if (a > largest_area) {
                largest_area = a;
                biggestApprox = approx;
            }
        }
    }
    return biggestApprox;
}

Mat extractPuzzle(Mat input, vector<Point> biggestApprox) {
    Mat outerBox = Mat(input.size(), CV_8UC1);
    cv::Point2f src_p[4];
    cv::Point2f dst_p[4];

    float w = (float) input.cols;
    float h = (float) input.rows;
    float hw = w / 2.0f;
    float hh = h / 2.0f;

    // from points
    src_p[0] = cv::Point2f(biggestApprox.at(1));
    src_p[1] = cv::Point2f(biggestApprox.at(0));
    src_p[2] = cv::Point2f(biggestApprox.at(3));
    src_p[3] = cv::Point2f(biggestApprox.at(2));

    // to points
    dst_p[0] = cv::Point2f(0.0f, 0.0f);
    dst_p[1] = cv::Point2f(w, 0.0f);
    dst_p[2] = cv::Point2f(w, h);
    dst_p[3] = cv::Point2f(0.0f, h);
//
    cv::Mat dst_img;

    // create perspective transform matrix
    cv::Mat trans_mat33 = cv::getPerspectiveTransform(src_p, dst_p); //CV_64F->double

    // perspective transform operation using transform matrix
    warpPerspective(input, outerBox, trans_mat33, input.size(), cv::INTER_LINEAR);
    return outerBox;
}

Mat getCell(Mat sudoku, int numCell) {
    Mat output = sudoku.clone();
    int y = sudoku.cols;
    int x = sudoku.rows;
    int cx = x / 9;
    int cy = y / 9;

    cout << "cell size is :" << cx <<  " * " << cy << " = " << cx*cy << endl;

    Rect rect = Rect((numCell % 9) * cy, (numCell / 9) * cx, cy, cx);
    return output(rect);
}


Mat removeLight(Mat img, Mat pattern, int method)
{
    Mat aux;
// if method is normalization
    if(method==1)
    {
// Require change our image to 32 float for division
        Mat img32, pattern32;
        img.convertTo(img32, CV_32F);
        pattern.convertTo(pattern32, CV_32F);
// Divide the image by the pattern
        aux= 1-(img32/pattern32);
        // Scale it to convert to 8bit format
        aux=aux*255;
// Convert 8 bits format
        aux.convertTo(aux, CV_8U);
    }else{
        aux= pattern-img;
    }
    return aux;
}

Mat calculateLightPattern(Mat img)
{
    Mat pattern;
// Basic and effective way to calculate the light pattern from one image
            blur(img, pattern, Size(img.cols/3,img.cols/3));
//    showImage(pattern);
    return pattern;
}

void ConnectedComponents(Mat img)
{
// Use connected components to divide our possibles parts of images
    Mat labels;
    int num_objects= connectedComponents(img, labels);
// Check the number of objects detected
    if(num_objects < 2 ){
        cout << "No objects detected" << endl;
        return;
    }else{
        cout << "Number of objects detected: " << num_objects - 1 << endl;
    }
    // Create output image coloring the objects
    Mat output= Mat::zeros(img.rows,img.cols, CV_8UC3);
    RNG rng( 0xFFFFFFFF );
    for(int i=1; i<num_objects; i++){
        Mat mask= labels==i;
        output.setTo(randomColor(rng), mask);
    }
    imshow("Result", output);
}


void FindContoursBasic(Mat img)
{
    vector<vector<Point> > contours;
    findContours(img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    Mat output= Mat::zeros(img.rows,img.cols, CV_8UC3);
// Check the number of objects detected
    if(contours.size() == 0 ){
        cout << "No objects detected" << endl;
        return;
    }else{
        cout << "Number of objects detected: " << contours.size() << endl;
    }
    RNG rng( 0xFFFFFFFF );
    for(int i=0; i<contours.size(); i++){
        drawContours(img, contours, i, randomColor(rng));
    }

    imshow("Result", img);
    waitKey(0);
}


void ConnectedComponentsStats(Mat cell)
{

    int cell_height = cell.rows;
    int cell_width = cell.cols;

    // setting parameters for long lines filtering
    float percent = 0.23;
    float width_threshold = cell_width - cell_width * percent;
    float height_threshold = cell_height - cell_height * percent;


    // Use connected components with stats
    Mat labels, stats, centroids;
    int num_objects= connectedComponentsWithStats(cell, labels, stats, centroids);
    // Check the number of objects detected
    if(num_objects < 2 ){
        cout << "No objects detected" << endl;
        return;
    }else{
        cout << "Number of objects detected: " << num_objects - 1 << endl;
    }

    // Create output image coloring the objects and show area
    Mat output= Mat::zeros(cell.rows,cell.cols, CV_8UC3);
    RNG rng( 0xFFFFFFFF );
    for(int i=1; i<num_objects; i++){
        int area = stats.at<int>(i, CC_STAT_AREA);
        int width = stats.at<int>(i, CC_STAT_WIDTH);
        int height = stats.at<int>(i, CC_STAT_HEIGHT);

        // filtering
        int boundingArea = width * height;
        if(width > width_threshold ) continue;
        if(height > height_threshold) continue;
        if(boundingArea < 220 || boundingArea > 900) continue;
        if(area < 110) continue; // area of the connected object

        Mat mask= labels==i;
        output.setTo(randomColor(rng), mask);
        showImage(output);
    }
}


int main(int argc, char **argv) {
    const char *files[] = {
            "../puzzles/sudoku.jpg",
            "../puzzles/sudoku1.jpg",
            "../puzzles/sudoku2.jpg",
            "../puzzles/sudoku3.jpg",
    };

    unsigned nb_files = sizeof(files) / sizeof(const char *);
    for (unsigned i = 0; i < nb_files; ++i) {
        Mat raw = imread(files[i], CV_LOAD_IMAGE_GRAYSCALE);
        Mat preprocessed = preprocess(raw.clone());
        vector<Point> biggestApprox = findBigestApprox(preprocessed);

        Mat sudoku = extractPuzzle(raw, biggestApprox);

        for(unsigned i = 0; i < 81 ; i++ ){
            Mat cell = getCell(sudoku, i), cell_no_noise, cell_no_light, final_cell, out;

            // remove noise
            medianBlur(cell, cell_no_noise, 1);
            // remove background/light
            cell_no_light = removeLight(cell_no_noise, calculateLightPattern(cell),2);
            // binarize image
            adaptiveThreshold(cell_no_light, final_cell, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);
            ConnectedComponentsStats(final_cell);
        }
    }

    return 0;
}
