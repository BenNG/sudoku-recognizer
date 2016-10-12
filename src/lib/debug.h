#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include "boost/filesystem.hpp"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
namespace fs = boost::filesystem;

int minuss(int i, int j);
Mat drawAllContour(Mat preprocessed);
Mat drawAllApprox(Mat preprocessed);
Mat drawAllApprox(Mat preprocessed, Mat original);
void drawMarkers(Mat input, vector<Point> biggestApprox);
Mat drawGrid(Mat input);
void showImage(Mat img);
fs::path getMyProjectRoot(fs::path p);
