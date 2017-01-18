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
    extractionInformation extractInfo, extractInfo2;
    string raw_features_path("./../assets/raw-features.yml"); // created by prepareData
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::READ);

    Ptr<ml::KNearest> knn = getKnn(raw_features);
 
    string filePath("./../assets/puzzles/s0.jpg"); // by default
    if (argc > 1)
    {
        filePath = argv[1];
    }

    Mat image = imread(filePath, CV_LOAD_IMAGE_GRAYSCALE);

    vector<Point> bigestApprox = findBigestApprox(image);
    extractInfo = extractPuzzle(image, bigestApprox);
    Mat extractedPuzzle = extractInfo.image;
    showImage(extractedPuzzle);

    Mat finalExtraction = recursiveExtraction(extractedPuzzle);
    showImage(finalExtraction);


    string resp = grabNumbers(finalExtraction, knn);

    cout << resp << endl;

    return 0;
}