#include <opencv2/opencv.hpp>
#include "puzzle.h"
#include "../debug.h"

using namespace cv;
using namespace std;

#ifndef UTILS_OPENCV_LIB
#define UTILS_OPENCV_LIB


// cells
Mat normalizeSize(Mat in, int size);
Mat extractNumber(Mat cell);
Mat prepareCell(Mat cell);
Mat removeLight(Mat img, Mat pattern, int method);
Mat calculateLightPattern(Mat img);
Mat ProjectedHistogram(Mat img, int t);
Mat features(Mat in, int sizeData);
Mat extractRoiFromCell(Mat sudoku, int k);

#endif //UTILS_OPENCV_LIB
