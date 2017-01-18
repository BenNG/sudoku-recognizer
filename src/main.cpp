#include "lib/sudoku.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace cv::ml;
using namespace std;

/**
 *
 * ----------------------------- Main ------------------------------------
 *
 * */
int main(int argc, char **argv)
{
    extractionInformation extractInfo;
    string raw_features_path("./../assets/raw-features.yml"); // created by prepareData
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::READ);

    Ptr<ml::KNearest> knn = getKnn(raw_features);
 
    string filePath("./../assets/puzzles/s0.jpg"); // by default
    if (argc > 1)
    {
        filePath = argv[1];
    }

    Mat image = imread(filePath, CV_LOAD_IMAGE_GRAYSCALE);

    extractInfo = extractPuzzle(image);
    Mat extractedPuzzle = extractInfo.image;

    string resp = grabNumbers(extractedPuzzle, knn);

    cout << resp << endl;

    return 0;
}