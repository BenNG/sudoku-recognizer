#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#ifndef DISPLAYIMAGE_MNIST_H
#define DISPLAYIMAGE_MNIST_H

typedef unsigned char       BYTE;

int readFlippedInteger(FILE *fp);
vector<Mat> readMNIST(bool training);
vector<Mat> readTrainingMNIST();
vector<Mat> readTestMNIST();

#endif //DISPLAYIMAGE_MNIST_H
