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

#include "lib/utils/opencv.h"
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
    Mat raw, sudoku;

    string imageName("./assets/puzzles/s0.jpg"); // by default
    if( argc > 1)
    {
        imageName = argv[1];
    }
    fs::path p(getPath(imageName));

    raw = imread(p.string(), CV_LOAD_IMAGE_GRAYSCALE);
    sudoku = extractPuzzle(raw);
    showImage(sudoku);
    
    Mat roi, normalized;
    for (int k = 0; k < 81; k++)
    {
        roi = extractRoiFromCell(sudoku, k);
        if (!roi.empty())
        {
            cout << k << endl;
            showImage(normalized);
        }
    }

    return 0;
}
