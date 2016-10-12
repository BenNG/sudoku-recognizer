#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif


#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

#include "../debug.h"
#include "extractPuzzle.h"


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
    Mat output;
    fs::path p(getMyProjectRoot(fs::current_path()));
    p /= "assets/puzzles/";

    if (fs::is_directory(p)) {
        fs::directory_iterator end_iter;
        for (fs::directory_iterator dir_itr(p);
             dir_itr != end_iter;
             ++dir_itr) {
                    string fullName = dir_itr->path().string();
                    Mat raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);

                    Mat sudoku = extractPuzzle(raw);

                    // sometimes the biggest area found is not correct, our puzzle is inside the extract image
                    // so we do it a second time to extract the biggest blob which is this time our puzzle
                    // this is the case for s6.jpg and s9.jpg for example

                    // Mat preprocessed2 = preprocess(sudoku.clone());
                    // vector<Point> biggestApprox2 = findBigestApprox(preprocessed2);
                    //
                    // if(!biggestApprox2.empty()){
                    //   sudoku = extractPuzzle(sudoku, biggestApprox2);
                    // }

                    // Mat sudoku2 = extractPuzzle(sudoku, biggestApprox2);
                    // output = drawAllApprox(preprocessed, raw);

                    showImage(sudoku);

        }
    }
    return 0;
}
