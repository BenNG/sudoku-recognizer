#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem.hpp"
#include "boost/progress.hpp"
#include "../mlp.h"
#include "../debug.h"
#include "opencv.h"
#include "opencv.h"
// #include "../preTraining/createDataForTraining.h"
#include "../preTraining/randomizeDataForTraining.h"
#include "../preTraining/featurizeDataForTraining.h"
#include <opencv2/opencv.hpp>

namespace fs = boost::filesystem;
using namespace cv;
using namespace cv::ml;
using namespace std;
using namespace boost;

/**
 * this function do too much things need to refact some day
 * */
string grab(string fileName);
