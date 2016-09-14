#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
/*
 * preprocess
 *
 * It's going to be easier for the opencv algorithm to detect the border of the sudoku
 */
Mat preprocess(Mat input){
    Mat outerBox = Mat(input.size(), CV_8UC1);
    GaussianBlur(input, input, Size(11, 11), 0);
    adaptiveThreshold(input, outerBox, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);
    bitwise_not(outerBox, outerBox);
    Mat kernel = (Mat_<uchar>(3,3) << 0,1,0,1,1,1,0,1,0);
    dilate(outerBox, outerBox, kernel);
    return outerBox;
}

int main(int argc, char **argv) {

    Mat sudoku = imread("puzzles/sudoku.jpg", 0);
    Mat original = imread("puzzles/sudoku.jpg", 0);

    preprocess(sudoku);

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", sudoku);
    waitKey(0);
    return 0;

}
