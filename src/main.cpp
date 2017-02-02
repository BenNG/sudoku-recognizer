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

    Mat original = imread(filePath, CV_LOAD_IMAGE_GRAYSCALE);

    // prepare knn
    string raw_features_path("./../assets/raw-features.yml"); // created by prepareData
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::READ);
    Ptr<ml::KNearest> knn = getKnn(raw_features);
    Ptr<ml::SVM> svm = getSvm(raw_features);

    Mat preprocessed = preprocess(original.clone());

    vector<Point> bigestApprox = findBiggestBlob(preprocessed, original);
    extractInfo = extractPuzzle(original, bigestApprox);
    Mat extractedPuzzle = extractInfo.image;
    Mat finalExtraction = recursiveExtraction(extractedPuzzle);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    ///////////////////////////////////////////////////////////////////////////////////////////////////////


    string initialStateOfTheSudoku = grabNumbers(finalExtraction, svm);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << duration/1000 << " ms" <<  endl;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    cout << (initialStateOfTheSudoku == "006400750005082060007306089050130900093000840002048070580209600070860200029003400") << endl;
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

    warpPerspective(writen, original, extractInfo.transformation, original.size(), WARP_INVERSE_MAP, BORDER_TRANSPARENT);

    showImage(original);

    return 0;
}