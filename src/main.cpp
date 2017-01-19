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

    vector<Point> bigestApprox = findBigestBlob(image);

    extractInfo = extractPuzzle(image, bigestApprox);
    Mat extractedPuzzle = extractInfo.image;
    // showImage(extractedPuzzle);

    Mat finalExtraction = recursiveExtraction(extractedPuzzle);
    // showImage(finalExtraction);

    string resp = grabNumbers(finalExtraction, knn);
    
    string solution = "121193000079000841050001000003050408005806300108030500000100080514000920000340050";

    Mat writen = writeOnPuzzle(finalExtraction, solution);

    warpPerspective(writen, image, extractInfo.transformation , image.size(), WARP_INVERSE_MAP, BORDER_TRANSPARENT);

    showImage(image);
    cout << resp << endl;

    return 0;
}