#include "debug.h"

int minuss(int i, int j)
{
    return i - j;
}

Mat drawAllContour(Mat preprocessed) {
    Mat output = Mat::zeros(preprocessed.rows, preprocessed.cols, preprocessed.type());
    Scalar white(255, 255, 255);
    vector < vector< Point > > contours;
    findContours(preprocessed, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    cout << "contours : " << contours.size() << endl;

    for (int i = 0; i < contours.size(); i++) {
        drawContours(output, contours, i, white, 2, 8);
    }

    return output;
}

Mat drawAllApprox(Mat preprocessed) {
    Mat output = Mat::zeros(preprocessed.rows, preprocessed.cols, preprocessed.type());

    Scalar white(255, 255, 255);
    vector<vector<Point> > contours;
    std::vector<cv::Point> approx;

    findContours(preprocessed.clone(), contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

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
    return output;
}
Mat drawAllApprox(Mat preprocessed, Mat origial) {
    Mat output = origial.clone();

    Scalar white(255, 255, 255);
    vector<vector<Point> > contours;
    std::vector<cv::Point> approx;

    findContours(preprocessed.clone(), contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

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
    return output;
}

void drawMarkers(Mat input, vector<Point> biggestApprox) {
    Scalar white(255, 255, 255);

    drawMarker(input, biggestApprox.at(0), white);
    drawMarker(input, biggestApprox.at(1), white);
    drawMarker(input, biggestApprox.at(2), white);
    drawMarker(input, biggestApprox.at(3), white);
}

Mat drawGrid(Mat input) {
    Scalar white(255, 255, 255);

    Mat output = input.clone();
    int h = input.cols;
    int w = input.rows;
    int cw = w / 9;
    int ch = h / 9;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            //  drawMarker(input, Point(j * ch, i * cw), white);
            Rect rect = Rect(j * ch, i * cw, ch, cw);
            rectangle(output, rect, white);
        }
    }
    return output;
}

void showImage(Mat img){
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", img);
    waitKey(0);
}

fs::path getMyProjectRoot(fs::path p){
    string projectRootFolderName = "sudoku";
    if(p.filename() == projectRootFolderName){
        return p;
    }else if (p.filename() == "/") {
        throw "could not find project root (in function getMyProjectRoot)";
    }else{
        return getMyProjectRoot(p.parent_path());
    }
}

/**
* from time to time there are some tiny text around the puzzle and it kills the detection
* this function remove the tiny contour 
*/
Mat removeTinyVolume(Mat input) {
    Mat output = input.clone();
    Scalar black(0, 0, 0);
    Scalar white(255, 255, 255);
    vector < vector< Point > > contours;
    findContours(input, contours, RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    cout << "contours : " << contours.size() << endl;

    for (int i = 0; i < contours.size(); i++) {
      if (contourArea(contours[i]) < 400){
        drawContours(output, contours, i, white, -1, 8);
      }
    }
    return output;
}
