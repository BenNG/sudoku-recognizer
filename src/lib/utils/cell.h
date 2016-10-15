#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#ifndef DISPLAYIMAGE_CELL_H
#define DISPLAYIMAGE_CELL_H

Mat normalizeSize(Mat in);

/**
 * extract the number and normelize the size
 * */
Mat extractNumber(Mat cell);


Mat prepareCell(Mat cell);
Mat removeLight(Mat img, Mat pattern, int method);

Mat calculateLightPattern(Mat img);

Mat ProjectedHistogram(Mat img, int t);

Mat features(Mat in, int sizeData);


#endif //DISPLAYIMAGE_CELL_H
