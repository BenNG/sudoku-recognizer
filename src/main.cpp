#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include "opencv2/text.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include "lib/mlp.h"
#include "lib/preTraining/createDataForTraining.h"
#include "lib/preTraining/randomizeDataForTraining.h"
#include "lib/preTraining/featurizeDataForTraining.h"
#include <opencv2/opencv.hpp>

namespace fs = boost::filesystem;
using namespace cv;
using namespace cv::ml;
using namespace std;
using namespace boost;

Scalar white(255, 255, 255);
Scalar green(0, 235, 0);
Scalar red(255, 0, 0);
Scalar blue(0, 0, 255);

Scalar randomColor(RNG &rng) {
    int icolor = (unsigned) rng;
    return Scalar(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);
}

Ptr<ANN_MLP> model;

const int hiddenLayerSize = 3;

/**
 * this function do too much things need to refact some day
 * */
string grab(Mat image){

    int cellValue;
    Mat cellFeatured;
    std::stringstream response;

    string message;
    fs::path featured("featuredDataForTraining.xml");
    fs::path trained_data("trained_data");

    if (!fs::exists(featured) && !fs::exists(trained_data)) {
        createData();

        cout << "Go clean the data folder !! say 'ok' when you are ready "  << endl;
        cin >> message;
        if(message == "ok"){
            std::cout << "2nd: randomize the file because we will use 80% of the data and 20% to test the network" << std::endl;
            randomizeData();
            std::cout << "3rd: extract feature for each cell and save it in featuredDataForTraining.xml" << std::endl;
            featurizeData();
        }
    }

    Ptr<ANN_MLP> model = build_mlp_classifier("featuredDataForTraining.xml", "trained_data");

    Mat preprocessed = preprocess(image.clone());
    vector<Point> biggestApprox = findBigestApprox(preprocessed);
    Mat sudoku = extractPuzzle(image, biggestApprox);

    for (unsigned i = 0; i < 81; i++) {
        Mat cell = getCell(sudoku, i), cell_no_noise, cell_no_light, final_cell;

        // remove noise
        medianBlur(cell, cell_no_noise, 1);
        // remove background/light
        cell_no_light = removeLight(cell_no_noise, calculateLightPattern(cell), 2);
        // binarize image
        adaptiveThreshold(cell_no_light, final_cell, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);

        Mat roi = extractRoiFromCell(final_cell);

        if (roi.empty()) {
            response << "0";
        } else {
            cellFeatured = features(roi, 15);
            cellValue = model->predict(cellFeatured);
//            cout << "r:" << cellValue + 1 << endl;
            response << cellValue + 1;
//            showImage(roi);
        }
    }

    Mat data;
    Mat responses;

    // fn
    FileStorage fs;
    fs.open("featuredDataForTraining.xml", FileStorage::READ);
    fs["TrainingDataF15"] >> data;
    fs["classes"] >> responses;
    // fn - end

//    // manual test
//    Mat sample = data.row(100);
//    float r = model->predict(sample);
//    cout << "r:" << r << endl;

    return response.str();
}


/**
 *
 * ----------------------------- Main ------------------------------------
 *
 * */
int main(int argc, char **argv) {

    string imageName("./assets/puzzles/s0.jpg"); // by default
    if( argc > 1)
    {
        imageName = argv[1];
    }
    Mat image;
    image = imread(imageName.c_str(), 0); // Read the file
    string response(grab(image));

    if( image.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    cout << response << endl;
    return 0;
}
