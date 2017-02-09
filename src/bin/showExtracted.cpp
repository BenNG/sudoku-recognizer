/**
    this file show the extrated sudoku for each file
    usage:

    # will show all extracted puzzles
    cmake .. && make && src/showExtracted

    # will show all extracted cells
    cmake .. && make && src/showExtracted --showCell

    # will show the extracted puzzle of `assets/puzzles/s33.jpg`
    cmake .. && make && src/showExtracted --puzzleNumber 33

    # will show all cells of `assets/puzzles/s33.jpg`
    cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell

    # will show the cell index = 10 of `assets/puzzles/s33.jpg`
    cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell --cellNumber 10

    # will show the cell index = 10 of `assets/puzzles/s33.jpg` plus the process of extraction
    cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell --cellNumber 10 --debug
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

    int K = 1;
    Mat response, dist, m;
    // load knn with "assets/config/raw-features.yml"
    string raw_features_path("./../assets/config/raw-features.yml"); // created by prepareData
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::READ);

    // Ptr<ml::KNearest> knn = getKnn(raw_features);
    // get cells values manually grabbed
    std::map<int, std::map<int, int>> knownCellValues(cellValues());

    bool showCell = false, showPuzzle = false, debug = false;
    int puzzleNumber = -1, cellNumber = -1;
    stringstream ss;
    ss << "./../assets/puzzles/";
    ExtractionInformation extractInfo;
    int kmin = 0, kmax = 81;


    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "--showCell")
        {
            showCell = true;
        }
        if (arg == "--cellNumber")
        {
            cellNumber = stoi(argv[i + 1]);
            kmin = cellNumber;
            kmax = cellNumber + 1;
        }
        if (arg == "--showPuzzle")
        {
            showPuzzle = true;
        }
        if (arg == "--puzzleNumber")
        {
            puzzleNumber = stoi(argv[i + 1]);
            ss << "s" << argv[i + 1] << ".jpg";
        }
        if (arg == "--debug")
        {
            debug = true;
        }
    }

    if (!showPuzzle && !showCell)
    {
        showPuzzle = true;
    }

    string p(ss.str());

    string fullName;
    Mat raw, sudoku, roi;

    vector<string> files;
    if (isDirectory(p.c_str()))
    {
        int num = getNumberOfFilesInFolder(p);
        for (int fileNumber = 0; fileNumber < num; fileNumber++)
        {
            stringstream ss;
            ss << "./../assets/puzzles/";
            ss << "s" << fileNumber << ".jpg";
            files.push_back(ss.str().c_str());
        }
    }
    else
    {
        files.push_back(p);
    }

    for (int fileNumber = 0; fileNumber < files.size(); fileNumber++)
    {
        cout << files[fileNumber] << endl;
        string fileName = files[fileNumber];

        raw = imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);

        Mat preprocessed = preprocess(raw.clone(), true);
        vector<Point> biggestApprox = findBiggestBlob(preprocessed, raw);
        extractInfo = extractPuzzle(raw, biggestApprox);
        Mat sudoku = recursiveExtraction(extractInfo.image);

        if (showCell)
        {
            for (int k = kmin; k < kmax; k++)
            {
                roi = extractRoiFromCell(sudoku, k, debug);
                if (!roi.empty())
                {
                    // cout << "cell index: " << k << endl;
                    // roi.convertTo(roi, CV_32F);
                    // knn->findNearest(roi.reshape(1, 1), K, noArray(), response, dist);
                    // cout << "resp: " << response << " expected: " << knownCellValues[fileNumber][k] << endl;
                    showImage(roi);
                }
            }
        }

        showImage(sudoku);
    }

    return 0;
}
