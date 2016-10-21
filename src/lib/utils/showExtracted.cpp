/**
this file show the extrated sudoku for each file
usage:

Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted

Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted assets/puzzles/s15.jpg

*/

#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

typedef unsigned char       BYTE;

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

#include "../hello.h"
#include "../knn.h"
#include "../debug.h"
#include "../mlp.h"
#include "puzzle.h"
#include "picture.h"
#include "cell.h"


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

    sayHello("LouLou");

    string fullName;
    Mat output, raw, preprocessed, sudoku;

    string path_str("assets/puzzles/"); // by default
    if( argc > 1)
    {
        path_str = argv[1];
    }

    Ptr<ml::KNearest> knn = getKnn();
    testKnn(knn);

    return 0;
}
