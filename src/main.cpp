#include "lib/sudoku.h"
#include <opencv2/opencv.hpp>
#include <memory>
#include <opencv2/core/ocl.hpp>


using namespace cv;
using namespace std; 

/**
 *
 * ----------------------------- Main ------------------------------------
 *
 * */
int main(int argc, char **argv)
{

    if (!cv::ocl::haveOpenCL())
    {
        cout << "OpenCL is not avaiable..." << endl;
        return 1;
    }
    cv::ocl::Context context;
    if (!context.create(cv::ocl::Device::TYPE_GPU))
    {
        cout << "Failed creating the context..." << endl;
        return 1;
    }

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