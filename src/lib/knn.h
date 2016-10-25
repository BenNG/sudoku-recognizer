#ifndef DISPLAYIMAGE_KNN_H
#define DISPLAYIMAGE_KNN_H

#include <string>
#include <sstream>      // std::stringstream
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "debug.h"
#include <time.h>

using namespace cv;
using namespace std;

typedef unsigned char       BYTE;

int readFlippedInteger(FILE *fp);
Ptr<ml::KNearest> getKnn();

void testKnn(Ptr<ml::KNearest> knn, bool debug);

#endif //DISPLAYIMAGE_KNN_H
