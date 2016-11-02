#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

using namespace cv;
using namespace cv::ml;
using namespace std;
namespace fs = boost::filesystem;

#ifndef UTILS_OPENCV_LIB
#define UTILS_OPENCV_LIB


// cells
Mat normalizeSize(Mat in, int size);
Mat extractNumber(Mat cell);
Mat prepareCell(Mat cell);
Mat removeLight(Mat img, Mat pattern, int method);
Mat calculateLightPattern(Mat img);
Mat ProjectedHistogram(Mat img, int t);
Mat features(Mat in, int sizeData);
Mat extractRoiFromCell(Mat sudoku, int k);
// mnist
int readFlippedInteger(FILE *fp);
vector<Mat> readMNIST(bool training);
vector<Mat> readTrainingMNIST();
vector<Mat> readTestMNIST();
// puzzle
Mat extractCell(Mat sudoku, int numCell);
// picture
Mat preprocess(Mat input);
vector<Point> findBigestApprox(Mat input);
Mat extractPuzzle(Mat input, vector<Point> biggestApprox);
Mat extractPuzzle(Mat input);
// knn
int readFlippedInteger(FILE *fp);
Ptr<ml::KNearest> getKnn();
void testKnn(Ptr<ml::KNearest> knn, bool debug);
// debug
int minuss(int i, int j);
Mat drawAllContour(Mat preprocessed);
vector<double> findBiggestComponent(Mat input);
Mat drawAllApprox(Mat preprocessed);
Mat drawAllApprox(Mat preprocessed, Mat original);
void drawMarkers(Mat input, vector<Point> biggestApprox);
Mat drawGrid(Mat input);
void showImage(Mat img);
fs::path getMyProjectRoot(fs::path p);
fs::path getPath(fs::path p);
Mat removeTinyVolume(Mat input, int area, Scalar color);
Mat deskew(Mat in);
// mlp
template<typename T> static Ptr<T> load_classifier(const string& persistence);
inline TermCriteria TC(int iters, double eps);
static void test_and_save_classifier(const Ptr<StatModel>& model,
                                     const Mat& data, const Mat& responses,
                                     int ntrain_samples, int rdelta,
                                     const string& filename_to_save);

Ptr<ANN_MLP> build_mlp_classifier(const fs::path data_filename, const fs::path persistence);


#endif //UTILS_OPENCV_LIB
