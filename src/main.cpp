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
#include "lib/createDataForTraining.h"
#include "lib/randomizeDataForTraining.h"
#include "lib/featurizeDataForTraining.h"
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
 *
 * ----------------------------- Main ------------------------------------
 *
 * */
int main(int argc, char **argv) {

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
    Mat data;
    Mat responses;

    // fn
    FileStorage fs;
    fs.open("featuredDataForTraining.xml", FileStorage::READ);
    fs["TrainingDataF15"] >> data;
    fs["classes"] >> responses;
    // fn - end

    // manual test
    Mat sample = data.row(100);
    float r = model->predict(sample);
    cout << "r:" << r << endl;

    return 0;
}
