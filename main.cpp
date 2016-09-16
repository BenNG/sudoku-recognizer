#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

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
vector<vector<Point> > findBigestContour(Mat input) {

    int largest_area = 0;
    int largest_contour_index = 0;
    vector<Point> contour;
    vector<vector<Point> > contours;

    findContours(input, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) {
        contour = contours[i];
        double a = contourArea(contour);
        if (a > largest_area) {
            largest_area = a;
            largest_contour_index = i;
        }
    }
    contour = contours[largest_contour_index];
    cout << contour << endl;
    std::vector<vector<Point> > biggest(1, contour);
    return biggest;
}

int main(int argc, char **argv) {
    const char *files[] = {"puzzles/sudoku.jpg", "puzzles/sudoku1.jpg", "puzzles/sudoku2.jpg", "puzzles/sudoku3.jpg"};
    vector<vector<Point> > contours;
    Scalar color(255, 255, 255);

    unsigned nb_files = sizeof(files) / sizeof(const char *);
    for (unsigned i = 0; i < nb_files; ++i) {
        Mat sudoku = imread(files[i], CV_LOAD_IMAGE_GRAYSCALE);
        Mat preprocessed = preprocess(sudoku.clone());
        std::vector<cv::Point> approx;

        findContours(preprocessed, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < contours.size(); i++) {

            // Approximate contour with accuracy proportional
            // to the contour perimeter
            cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

            // Skip small or non-convex objects
            if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
                continue;

            std::vector<vector<Point> > approx_contour(1, approx);
            drawContours(sudoku, approx_contour, 0, color, 2, 8);


        }
        //        vector< vector<Point> > biggestContour = findBigestContour(preprocessed);


        namedWindow("Display Image", WINDOW_AUTOSIZE);
        imshow("Display Image", sudoku);
        waitKey(0);
    }

    return 0;
}
