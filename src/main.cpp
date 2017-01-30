#include "lib/sudoku.h"
#include <opencv2/opencv.hpp>
#include <memory>

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

    ExtractionInformation extractInfo, extractInfo2;

    string filePath("./../assets/puzzles/s0.jpg"); // by default
    if (argc > 1)
    {
        filePath = argv[1];
    }

    Mat image = imread(filePath, CV_LOAD_IMAGE_GRAYSCALE);

    // prepare knn
    string raw_features_path("./../assets/raw-features.yml"); // created by prepareData
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::READ);
    Ptr<ml::KNearest> knn = getKnn(raw_features);





    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    ///////////////////////////////////////////////////////////////////////////////////////////////////////


    vector<Point> bigestApprox = findBiggestBlob(image);
    extractInfo = extractPuzzle(image, bigestApprox);
    Mat extractedPuzzle = extractInfo.image;
    Mat finalExtraction = recursiveExtraction(extractedPuzzle);
    string initialStateOfTheSudoku = grabNumbers(finalExtraction, knn);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    cout << duration << endl;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////



    // showImage(extractedPuzzle);

    // mouline: 697
    // getknn 162
    // findBiggestBlob 35 -> 23

    // recursiveExtraction 91
    // grabNumbers 128


    string solution;

    Sudoku::init();
    if (auto S = solve(unique_ptr<Sudoku>(new Sudoku(initialStateOfTheSudoku))))
    {
        solution = S->getSolution();
        cout << solution;
    }
    else
    {
        cout << "No solution";
    }
    cout << endl;

    Mat writen = writeOnPuzzle(finalExtraction, initialStateOfTheSudoku, solution);

    warpPerspective(writen, image, extractInfo.transformation, image.size(), WARP_INVERSE_MAP, BORDER_TRANSPARENT);

    showImage(image);

    return 0;
}