#include <opencv2/opencv.hpp>
#include "../debug.h"

using namespace cv;
using namespace std;

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

#endif //UTILS_OPENCV_LIB
