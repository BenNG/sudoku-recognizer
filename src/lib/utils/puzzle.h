#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Created by benng on 30/09/16.
//

#ifndef DISPLAYIMAGE_CELLHANDLING_H
#define DISPLAYIMAGE_CELLHANDLING_H



/**
 * get cell from an image created by the picture fn
 * */
Mat extractCell(Mat sudoku, int numCell);
#endif //DISPLAYIMAGE_CELLHANDLING_H
