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
    bool cells = false;
    if (argc > 1)
    {
        if(argv[1] == "cells"){
            cells = true;
        }
    }
    sayHello("LouLou");

    string fullName;
    Mat raw, sudoku;

    fs::path p(getPath("assets/puzzles/"));

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


            for (int k = 0; k < 81; k++)
            {
                Mat roi = extractRoiFromCell(sudoku, k);

                if (!roi.empty())
                {
                    Mat normalized = normalizeSize(roi, 28);
                    showImage(normalized);
                }
            }

        }
    }
    else
    {
        cout << "put your images in assets/images" << endl;
    }

    return 0;
}
