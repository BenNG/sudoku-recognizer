/**
this module handle the original picture
you can preprocess, find the biggest contour and etract it in order to get the puzzle then you can use the puzzle module.
*/


#include "picture.h"

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
    outerBox = removeTinyVolume(outerBox.clone());
    bitwise_not(outerBox, outerBox);
    dilate(outerBox, outerBox, Mat());
    return outerBox;
}

    // sort using a custom function object
    struct str{
      bool operator() ( Point2f a, Point2f b ){
          return a.x <= b.x;
      }
    } sort_xs;

    // sort using a custom function object
    struct str2{
      bool operator() ( Point2f a, Point2f b ){
          return a.y <= b.y;
      }
    } sort_ys;

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

    cv::Point2f tl;
    cv::Point2f tr;
    cv::Point2f bl;
    cv::Point2f br;

    cv::Point2f xs[4];
    cv::Point2f ys[4];

    cv::Point2f tops[2];
    cv::Point2f bottoms[2];

    cv::Point2f src_p[4];
    cv::Point2f dst_p[4];

    xs[0] = biggestApprox.at(0);
    xs[1] = biggestApprox.at(1);
    xs[2] = biggestApprox.at(2);
    xs[3] = biggestApprox.at(3);

    ys[0] = biggestApprox.at(0);
    ys[1] = biggestApprox.at(1);
    ys[2] = biggestApprox.at(2);
    ys[3] = biggestApprox.at(3);

    std::sort(ys, ys + 4, sort_ys);

    tops[0] = ys[0];
    tops[1] = ys[1];

    std::sort(tops, tops + 2, sort_xs);

    tl = tops[0];
    tr = tops[1];

    bottoms[0] = ys[2];
    bottoms[1] = ys[3];

    std::sort(bottoms, bottoms + 2, sort_xs);

    bl = bottoms[0];
    br = bottoms[1];

    //
    // cout << tl << endl;
    // cout << tr << endl;
    // cout << bl << endl;
    // cout << br << endl;
    // cout << "------------------------------------" << endl;

    float w = (float) input.cols;
    float h = (float) input.rows;
    float hw = w / 2.0f;
    float hh = h / 2.0f;

    // from points
    src_p[0] = tl;
    src_p[1] = tr;
    src_p[2] = br;
    src_p[3] = bl;

    // to points
    dst_p[0] = cv::Point2f(0.0f, 0.0f);
    dst_p[1] = cv::Point2f(w, 0.0f);
    dst_p[2] = cv::Point2f(w, h);
    dst_p[3] = cv::Point2f(0.0f, h);

    cv::Mat dst_img;

    // create perspective transform matrix
    cv::Mat trans_mat33 = cv::getPerspectiveTransform(src_p, dst_p); //CV_64F->double

    // perspective transform operation using transform matrix
    warpPerspective(input, outerBox, trans_mat33, input.size(), cv::INTER_LINEAR);
    return outerBox;
}

Mat extractPuzzle(Mat original) {
    Mat outerBox = Mat(original.size(), CV_8UC1);

    Mat preprocessed = preprocess(original.clone());
    vector<Point> biggestApprox = findBigestApprox(preprocessed);

    outerBox = extractPuzzle(original, biggestApprox);


    // trick
    // sometimes the biggest area found is not correct, our puzzle is inside the extract image
    // so we do it a second time to extract the biggest blob which is this time our puzzle
    // this is the case for s6.jpg and s9.jpg for example

    Mat preprocessed2 = preprocess(outerBox.clone());
    vector<Point> biggestApprox2 = findBigestApprox(preprocessed2);

    if(!biggestApprox2.empty()){
      outerBox = extractPuzzle(outerBox);
    }
    // trick - end

    return outerBox;
}
