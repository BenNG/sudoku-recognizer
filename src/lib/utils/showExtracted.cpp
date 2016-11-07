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

#include <map>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include <boost/program_options.hpp>

#include "../hello.h"
#include "opencv.h"

#include <opencv2/opencv.hpp>

namespace po = boost::program_options;
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

    int K = 1;
    Mat response, dist, m;

    Ptr<ml::KNearest> knn = getKnn();

    std::map<int, std::map<int, int>> cellV(cellValues());

    bool showCell = false, showPuzzle = false, debug = false;
    int puzzleNumber, cellNumber;
    stringstream ss;
    ss << "assets/puzzles/";
    po::options_description desc("Allowed options");
    desc.add_options()("debug", "debug")("showPuzzle", "show puzzles")("showCell", "show cells")("puzzleNumber", po::value<int>(&puzzleNumber)->default_value(-1))("cellNumber", po::value<int>(&cellNumber)->default_value(-1))("puzzle", po::value<int>(&puzzleNumber)->default_value(-1));

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << "\n";
        return 0;
    }

    if (vm.count("showCell"))
    {
        showCell = true;
    }

    if (vm.count("debug"))
    {
        debug = true;
    }

    if (vm.count("showPuzzle"))
    {
        showPuzzle = true;
    }

    if (puzzleNumber != -1)
    {
        ss << "s" << puzzleNumber << ".jpg";
    }

    if (!showPuzzle && !showCell)
    {
        showPuzzle = true;
    }

    fs::path p(getPath(ss.str()));


    bool cells = false;
    if (argc > 1)
    {
        if (argv[1] == "cells")
        {
            cells = true;
        }
    }
    sayHello("LouLou");

    string fullName;
    Mat raw, sudoku, roi;

    if (fs::is_directory(p))
    {
        fs::directory_iterator end_iter;
        for (fs::directory_iterator dir_itr(p);
             dir_itr != end_iter;
             ++dir_itr)
        {
            fullName = dir_itr->path().string();
            int fileNumber = stoi(dir_itr->path().stem().string().substr(1));

            cout << fullName << endl;
            raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
            sudoku = extractPuzzle(raw);

            if (showPuzzle)
            {
                showImage(sudoku);
            }
            if (showCell || cellNumber != -1)
            {
                if (cellNumber != -1)
                {

                    roi = extractRoiFromCell(sudoku, cellNumber, debug);
                    if (!roi.empty())
                    {

                        cout << "cell index: " << cellNumber << endl;
                        roi.convertTo(roi, CV_32F);

                        knn->findNearest(roi.reshape(1, 1), K, noArray(), response, dist);
                        cout << "resp: " << response << " expected: " << cellV[fileNumber][cellNumber] << endl;
                        showImage(roi);
                    }
                }
                else
                {

                    for (int k = 0; k < 81; k++)
                    {
                        roi = extractRoiFromCell(sudoku, k, debug);
                        if (!roi.empty())
                        {
                            cout << "cell index: " << k << endl;
                            roi.convertTo(roi, CV_32F);
                            knn->findNearest(roi.reshape(1, 1), K, noArray(), response, dist);
                            cout << "resp: " << response << " expected: " << cellV[fileNumber][k] << endl;
                            showImage(roi);
                        }
                    }
                }
            }
        }
    }
    else
    {
        cout << p << endl;
        raw = imread(p.string(), CV_LOAD_IMAGE_GRAYSCALE);
        sudoku = extractPuzzle(raw);
        if (showPuzzle)
        {
            showImage(sudoku);
        }
        if (showCell || cellNumber != -1)
        {
            if (cellNumber != -1)
            {

                roi = extractRoiFromCell(sudoku, cellNumber, debug);
                if (!roi.empty())
                {
                    cout << "cell index: " << cellNumber << endl;
                    roi.convertTo(roi, CV_32F);

                    knn->findNearest(roi.reshape(1, 1), K, noArray(), response, dist);
                    cout << "resp: " << response << " expected: " << cellV[puzzleNumber][cellNumber] << endl;
                    showImage(roi);
                }
                // showCells(sudoku, cellNumber, debug);
            }
            else
            {
                for (int k = 0; k < 81; k++)
                {
                    roi = extractRoiFromCell(sudoku, k, debug);
                    if (!roi.empty())
                    {
                        cout << "cell index: " << k << endl;
                        roi.convertTo(roi, CV_32F);

                        knn->findNearest(roi.reshape(1, 1), K, noArray(), response, dist);
                        cout << "resp: " << response << " expected: " << cellV[puzzleNumber][k] << endl;
                        showImage(roi);
                    }
                }
                // showCells(sudoku, debug);
            }
        }
    }

    return 0;
}
