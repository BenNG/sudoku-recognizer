#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char **argv) {

    Mat sudoku = imread("puzzles/sudoku.jpg", 0);
    Mat original = imread("puzzles/sudoku.jpg", 0);

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", sudoku);
    waitKey(0);
    return 0;

}
