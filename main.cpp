#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Scalar white(255, 255, 255);

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
 * Find the biggest contour in the image
 * note that it returns vector< vector<Point> > because it is more convenient to use drawContours after
 * */
vector<vector<Point> > findBigestApprox(Mat input) {

    int largest_area = 0;
    vector<vector<Point> > contours;
    vector<cv::Point> approx;
    vector<cv::Point> biggestApprox;

    findContours(input, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) {

        // Approximate contour with accuracy proportional
        // to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.1, true);

        // Skip small or non-convex objects
        if (std::fabs(contourArea(contours[i])) < 1200 || !isContourConvex(approx))
            continue;

        if (approx.size() == 4){
            double a = contourArea(contours[i]);
            if (a > largest_area) {
                largest_area = a;
                biggestApprox = approx;
            }
        }

    }

    vector<vector<Point> > biggestApproxContainer(1, biggestApprox);
    return biggestApproxContainer;
}

int main(int argc, char **argv) {
    const char *files[] = {"puzzles/sudoku.jpg", "puzzles/sudoku1.jpg", "puzzles/sudoku2.jpg", "puzzles/sudoku3.jpg"};

    unsigned nb_files = sizeof(files) / sizeof(const char *);
    for (unsigned i = 0; i < nb_files; ++i) {
        Mat sudoku = imread(files[i], CV_LOAD_IMAGE_GRAYSCALE);


        Mat preprocessed = preprocess(sudoku.clone());
        vector<vector<Point> > biggestApprox = findBigestApprox(preprocessed);

        drawMarker(sudoku, biggestApprox[0].at(0), white);
        drawMarker(sudoku, biggestApprox[0].at(1), white);
        drawMarker(sudoku, biggestApprox[0].at(2), white);
        drawMarker(sudoku, biggestApprox[0].at(3), white);

        namedWindow("Display Image", WINDOW_AUTOSIZE);
        imshow("Display Image", sudoku);
        waitKey(0);
    }

    return 0;
}
