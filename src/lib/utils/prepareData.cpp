#include <iostream>
#include <regex>

#include "opencv.h"

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

    fs::path raw_features_path(getPath("assets/raw-features.yml"));

    int incrCell = 0; // --> 1184
    Mat roi, normalized;
    for (int i = 0; i <= lastTrainingPuzzle; i++)
    {
        // cout << i << endl;
        stringstream ss;
        ss << "assets/puzzles/s";
        ss << i;
        ss << ".jpg";
        fs::path p(getPath(ss.str()));

        fullName = p.string();
        // cout << fullName << endl;

        raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
        sudoku = extractPuzzle(raw);


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

    cv::FileStorage raw_features(raw_features_path.string(), cv::FileStorage::WRITE); // open the classifications file

    raw_features << "features" << features;
    raw_features << "labels" << labels;
    raw_features.release();

    return 0;
}
