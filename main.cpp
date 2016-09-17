#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Scalar white(255, 255, 255);
Scalar green(0, 235, 0);
Scalar red(255, 0, 0);
Scalar blue(0, 0, 255);

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
vector<Point> findBigestApprox(Mat input) {

    int largest_area = 0;
    vector<vector<Point> > contours;
    vector<cv::Point> approx;
    vector<cv::Point> biggestApprox;

    findContours(input, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) {

        // Approximate contour with accuracy proportional
        // to the contour perimeter
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

/*
 * to help debug
 * */
void drawAllContour(Mat input, Mat output) {
    vector<vector<Point> > contours;
    findContours(input, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) {
        drawContours(output, contours, i, white, 2, 8);
    }
}

/*
 * to help debug
 * */
void drawAllApprox(Mat input, Mat output) {
    vector<vector<Point> > contours;
    std::vector<cv::Point> approx;

    findContours(input, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) {

        // Approximate contour with accuracy proportional
        // to the contour perimeter
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.1, true);

        // Skip small or non-convex objects
        if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
            continue;

        std::vector<vector<Point> > approx_contour(1, approx);
        drawContours(output, approx_contour, 0, white, 2, 8);
    }
}

/*
 * to help debug
 * */
void drawMarkers(Mat input, vector < Point > biggestApprox) {
    drawMarker(input, biggestApprox.at(0), white);
    drawMarker(input, biggestApprox.at(1), white);
    drawMarker(input, biggestApprox.at(2), white);
    drawMarker(input, biggestApprox.at(3), white);
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

int main(int argc, char **argv) {
    const char *files[] = {"puzzles/sudoku.jpg", "puzzles/sudoku1.jpg", "puzzles/sudoku2.jpg", "puzzles/sudoku3.jpg"};

    unsigned nb_files = sizeof(files) / sizeof(const char *);
    for (unsigned i = 0; i < nb_files; ++i) {
        Mat sudoku = imread(files[i], CV_LOAD_IMAGE_GRAYSCALE);
        Mat original = imread(files[i]);


        Mat preprocessed = preprocess(sudoku.clone());
        vector<Point> biggestApprox = findBigestApprox(preprocessed);
//        drawAllContour(preprocessed, sudoku);
//        drawAllApprox(preprocessed, sudoku);
//        drawMarkers(sudoku, biggestApprox);
        Mat extractedPuzzle = extractPuzzle(sudoku, biggestApprox);

        int h = extractedPuzzle.cols;
        int w = extractedPuzzle.rows;
        int cw = w / 9;
        int ch = h / 9;

        for (int i = 0; i < 1; i++) {
            for (int j = 0; j < 9; j++) {
//                drawMarker(extractedPuzzle, Point(j * ch, i * cw), white);
                Rect rect = Rect(j * ch, i * cw, ch, cw);
                rectangle(extractedPuzzle, rect, white);

            }
        }

        drawMarker(extractedPuzzle, Point(5 * ch, 1 * cw), white);

        namedWindow("Display Image", WINDOW_AUTOSIZE);
        imshow("Display Image", extractedPuzzle);
        waitKey(0);
    }

    return 0;
}
