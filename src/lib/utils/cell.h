#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Created by benng on 30/09/16.
//

#ifndef DISPLAYIMAGE_CELLHANDLING_H
#define DISPLAYIMAGE_CELLHANDLING_H



/**
 * get cell from an image created by the extractPuzzle fn
 * */
Mat getCell(Mat sudoku, int numCell);
Mat prepareCell(Mat cell);
Mat removeLight(Mat img, Mat pattern, int method);

Mat calculateLightPattern(Mat img);

Mat normalizeSize(Mat in);
/**
 * extract the number and normelize the size
 * */
Mat extractRoiFromCell(Mat cell);

Mat ProjectedHistogram(Mat img, int t);

Mat features(Mat in, int sizeData);


#endif //DISPLAYIMAGE_CELLHANDLING_H
