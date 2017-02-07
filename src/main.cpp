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

    Mat result = mouline(original);

    showImage(result);

    return 0;
}