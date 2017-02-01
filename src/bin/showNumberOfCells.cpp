/**
display the number of non empty cell
*/

#include <map>

#include "../lib/hello.h"
#include "../lib/sudoku.h"

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

    int numberOfCells = 0;

    Mat response, dist, m;
    // load knn with "assets/raw-features.yml"
    string raw_features_path("./../assets/raw-features.yml"); // created by prepareData
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::READ);

    Ptr<ml::KNearest> knn = getKnn(raw_features);

    bool debug = false;
    stringstream ss;
    ss << "./../assets/puzzles/";
    ExtractionInformation extractInfo;

    string p(ss.str());

    string fullName;
    Mat raw, sudoku, roi;

    if (isDirectory(p.c_str()))
    {
        for (int fileNumber = 0; fileNumber <= lastTrainingPuzzle; fileNumber++)
        {
            stringstream ss;
            ss << "./../assets/puzzles/";
            ss << "s" << fileNumber << ".jpg";
            fullName = ss.str();

            cout << fullName << endl;

            raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);

            Mat preprocessed = preprocess(raw.clone());

            vector<Point> biggestApprox = findBiggestBlob(preprocessed);
            extractInfo = extractPuzzle(raw, biggestApprox);
            Mat sudoku = recursiveExtraction(extractInfo.image);

            for (int k = 0; k < 81; k++)
            {
                roi = extractRoiFromCell(sudoku, k, debug);
                if (!roi.empty())
                {
                    numberOfCells = numberOfCells + 1;
                }
            }
        }
    }

    cout << "numberOfCells: " << numberOfCells << endl;

    return 0;
}
