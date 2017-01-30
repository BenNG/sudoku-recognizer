#include <iostream>
#include <regex>

#include "../lib/sudoku.h"

using namespace std; // Or using std::string;

// -----------------------------------------------------------------
// -----------------------------------------------------------------
// ------------------- from s0 to s35 ------------------------------
// -----------------------------------------------------------------
// ---------- create both features and labels files here -----------

int main(int argc, char **argv)
{

    // data to return
    Mat features(nbrOfCells, normalizedSizeForCell * normalizedSizeForCell, CV_8UC1);
    Mat labels(1, nbrOfCells, CV_8UC1);

    // Ptr<ml::KNearest> knn(ml::KNearest::create());
    std::map<int, std::map<int, int>> cellV(cellValues());
    int value;

    string fullName;
    Mat raw, sudoku;
    ExtractionInformation extractInfo;

    string raw_features_path("./../../assets/raw-features.yml");
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::WRITE); // open the classifications file

    int incrCell = 0; // --> 1184
    Mat roi, normalized;
    for (int i = 0; i <= lastTrainingPuzzle; i++)
    {
        // cout << i << endl;
        stringstream ss;
        ss << "./../assets/puzzles/s";
        ss << i;
        ss << ".jpg";
        string fullName(ss.str());

        raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
        
        vector<Point> biggestApprox = findBiggestBlob(raw);
        extractInfo = extractPuzzle(raw, biggestApprox);
        Mat sudoku = recursiveExtraction(extractInfo.image);


        for (int k = 0; k < 81; k++)
        {
            roi = extractRoiFromCell(sudoku, k);
            if (!roi.empty())
            {
                value = cellV[i][k];
                // cout << k << ":" << value << endl;
                // featurize
                Mat feat = roi.reshape(1, 1);

                feat.copyTo(features.row(incrCell));
                labels.at<unsigned char>(0, incrCell) = value;

                incrCell++;
            }
        }
    }

    features.convertTo(features, CV_32F);
    labels.convertTo(labels, CV_32F);

    raw_features << "features" << features;
    raw_features << "labels" << labels;
    raw_features.release();

    return 0;
}
