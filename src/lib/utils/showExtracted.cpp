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
    // load knn with "assets/raw-features.yml"
    Ptr<ml::KNearest> knn = getKnn();
    // get cells values manually grabbed
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



    string p(getPath(ss.str()));

    string fullName;
    Mat raw, sudoku, roi;

    if (isDirectory(p.c_str()))
    {
        int num = getNumberOfFilesInFolder(p);
        cout << num << endl;

        for (int fileNumber = 0; fileNumber < num; fileNumber++)
        {

            stringstream ss;
            ss << "assets/puzzles/";
            ss << "s" << fileNumber << ".jpg";
            fullName = getPath(ss.str());

            cout << fullName << endl;
            raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
            sudoku = extractPuzzle(raw);

            if (showPuzzle)
            {
                showImage(sudoku);
            }
            // all files - one cell
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
                // all files - all cell
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
        raw = imread(p, CV_LOAD_IMAGE_GRAYSCALE);
        sudoku = extractPuzzle(raw);
        if (showPuzzle)
        {
            showImage(sudoku);
        }
        // one file - all cell
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
            }
            // one file - all cell
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
            }
        }
    }

    return 0;
}
