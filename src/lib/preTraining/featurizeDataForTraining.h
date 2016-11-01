/**
 * 3/3 create data for the classifier
 * this script will create featuredDataForTraining.xml
 *
 * Each cell is represented by a group of features like ProjectedHistogram...
 * Each cell has an associated value (trainingLabels)
 *
 * */
#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <opencv2/opencv.hpp>

#include "../utils/opencv.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

#include <iostream>
using namespace cv;
using namespace std;

int featurizeData();