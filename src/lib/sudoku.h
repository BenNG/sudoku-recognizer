#ifndef UTILS_OPENCV_LIB
#define UTILS_OPENCV_LIB

#include <map>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>

#include "whereami.h"

#include <chrono>
#include <memory>

#include <string>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <thread>

using namespace cv;
using namespace cv::ml;
using namespace std;

class ExtractionInformation
{
public:
  Mat image;
  Mat transformation;
};

// Training
static const int lastTrainingPuzzle = 35; // change this if you added some new pictures and you want to generate a new assets/raw-features.yml

// HOG parameters for training that for some reason are not included in the HOG class
static const Size trainingPadding = Size(0, 0);
static const Size winStride = Size(8, 8);

// knn testing attributes
static const int nbrOfCells = 1186;
static const double percent = 0.9;

// cells
static const int normalizedSizeForCell = 32;
Mat normalizeSize(Mat in, int size);
Mat extractNumber(Mat cell);
Mat extractNumber(Mat cell, bool debug);
Mat prepareCell(Mat cell);
Mat removeLight(Mat img, Mat pattern, int method);
Mat calculateLightPattern(Mat img);
Mat ProjectedHistogram(Mat img, int t);
Mat features(Mat in, int sizeData);
Mat extractRoiFromCell(Mat sudoku, int k);
Mat extractRoiFromCell(Mat sudoku, int k, bool debug);

// mnist
int readFlippedInteger(FILE *fp);
// puzzle
Mat extractCell(Mat sudoku, int numCell);
// picture
Mat preprocess(Mat input, bool tiny);
vector<Point> findBiggestBlob(Mat preprocessed, Mat original /* debug purpose*/);
Mat writeOnPuzzle(Mat puzzle, string initialState, string solution);
std::vector<Point2f> getSudokuCoordinates(Mat input, vector<Point> biggestApprox);

Mat recursiveExtraction(Mat input);
ExtractionInformation extractPuzzle(Mat input, vector<Point> biggestApprox);
// knn
int readFlippedInteger(FILE *fp);
Ptr<ml::KNearest> getKnn(FileStorage raw_features);
Ptr<ml::SVM> getSvm(FileStorage raw_features);
void testKnn(Ptr<ml::KNearest> knn, cv::FileStorage raw_features);
// debug
int minuss(int i, int j);
Mat drawAllContour(Mat preprocessed);
Mat drawAllContour(Mat preprocessed, vector<vector<Point>> contours);
bool isContourValid(int contourAreaValue, vector<Point> approx);
vector<double> findBiggestComponent(Mat input);
Mat drawAllApprox(Mat preprocessed);
Mat drawAllApprox(Mat preprocessed, Mat original);
void drawMarkers(Mat input, vector<Point> biggestApprox);
Mat drawGrid(Mat input);
void showContour(Mat img, vector<Point> contour);
void showImage(Mat img);
string getMyProjectRoot(string projectRootName, string path);
// string getPath(string p);
Mat removeTinyVolume(Mat input, int area, Scalar color);
Mat deskew(Mat in);
void showCells(Mat sudoku, bool debug);
void showCells(Mat sudoku, int cellNum, bool debug);
// mlp
template <typename T>
static Ptr<T> load_classifier(const string &persistence);
inline TermCriteria TC(int iters, double eps);
static void test_and_save_classifier(const Ptr<StatModel> &model,
                                     const Mat &data, const Mat &responses,
                                     int ntrain_samples, int rdelta,
                                     const string &filename_to_save);

// sudoku
// string grabNumbers(Mat extractedPuzzle, Ptr<ml::SVM> svm);
void grabNumbers(Mat extractedPuzzle, Ptr<ml::SVM> svm, string *result);
void _grabNumbers(Mat extractedPuzzle, Ptr<ml::SVM> svm, string *result, vector<int> tab);

void _grabNumber(Mat extractedPuzzle, int cellNumber, Ptr<ml::SVM> svm, string *pResult);
// file system
// std::string getexepath();
string joinPath(vector<string> strs);
vector<string> splitPath(string path);
int isDirectory(const char *path);
int getNumberOfFilesInFolder(string dir);
int getdir(string dir, vector<string> &files);

std::map<int, std::map<int, int>> cellValues();
Mat mouline(Mat original);

// debug
string hello();
// string askServerForSolution(string initialSudokuState);

std::pair<bool, std::array<int, 81>> solve(const char *input);

bool areSameNonZeroValues(string init, string sol);

void call_from_thread();

#endif //UTILS_OPENCV_LIB
