/**
this file show the extrated sudoku for each file
usage:

Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted

Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted assets/puzzles/s15.jpg

*/

#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif

typedef unsigned char BYTE;

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

#include "../hello.h"
#include "../knn.h"
#include "../debug.h"
#include "../mlp.h"
#include "puzzle.h"
#include "picture.h"
#include "opencv.h"

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
int main(int argc, char **argv)
{

    sayHello("LouLou");

    string fullName;
    Mat output, raw, preprocessed, sudoku;

    int K = 1;
    Mat response, dist;

    string path_str("assets/puzzles/"); // by default
    if (argc > 1)
    {
        path_str = argv[1];
    }

    fs::path p(getPath(path_str));

    if (fs::is_directory(p))
    {
        fs::directory_iterator end_iter;
        for (fs::directory_iterator dir_itr(p);
             dir_itr != end_iter;
             ++dir_itr)
        {
            fullName = dir_itr->path().string();
            cout << fullName << endl;

            raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);

            sudoku = extractPuzzle(raw);

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
    else
    {
        cout << "please give a folder as a parameter" << endl;
    }

    return 0;
}
