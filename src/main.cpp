#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include "opencv2/text.hpp"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

#include "lib/utils/opencv.h"
#include "lib/preTraining/randomizeDataForTraining.h"
#include "lib/preTraining/featurizeDataForTraining.h"
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
    Mat raw, sudoku;
    Ptr<ml::KNearest> knn = getKnn();
    stringstream ss;
    int K = 1;
    Mat response, dist, m;


    string filePath("assets/puzzles/s0.jpg"); // by default
    if (argc > 1)
    {
        filePath = argv[1];
    }

    string resp = grab(filePath);

    // cout << resp << endl;

    // raw = imread(p.string(), CV_LOAD_IMAGE_GRAYSCALE);
    // sudoku = extractPuzzle(raw);

    // Mat roi;
    // for (int k = 0; k < 81; k++)
    // {
    //     roi = extractRoiFromCell(sudoku, k);
    //     if (!roi.empty())
    //     {
    //         // cout << k << endl;
    //         roi.convertTo(roi, CV_32F);
    //         knn->findNearest(roi.reshape(1, 1), K, noArray(), response, dist);

    //         // cout << "resp: " << response << endl;
    //         ss << response.at<float>(0);
    //         // showImage(roi);
    //     }
    //     else
    //     {
    //         ss << "0";
    //     }
    // }

    // cout << resp << endl;

    return 0;
}