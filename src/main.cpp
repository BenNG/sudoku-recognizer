#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include "opencv2/text.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include "lib/mlp.h"
#include "lib/preTraining/createDataForTraining.h"
#include "lib/preTraining/randomizeDataForTraining.h"
#include "lib/preTraining/featurizeDataForTraining.h"
#include "lib/utils/sudoku.h"
#include <opencv2/opencv.hpp>

namespace fs = boost::filesystem;
using namespace cv;
using namespace cv::ml;
using namespace std;
using namespace boost;

/**
 *
 * ----------------------------- Main ------------------------------------
 *
 * */
int main(int argc, char **argv) {

    string imageName("./assets/puzzles/s0.jpg"); // by default
    if( argc > 1)
    {
        imageName = argv[1];
    }
    Mat image;
    image = imread(imageName.c_str(), 0); // Read the file
    string response(grab(image));

    if( image.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    cout << response << endl;
    return 0;
}
