#ifndef UTILS_OPENCV_LIB
#define UTILS_OPENCV_LIB

#include <map>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>

#include "whereami.h"

#include <string>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include <curl/curl.h>


using namespace cv;
using namespace cv::ml;
using namespace std;

struct extractionInformation {
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
Mat preprocess(Mat input);
vector<Point> findBigestBlob(Mat input);
Mat writeOnPuzzle(Mat puzzle, string solution);
std::vector<Point2f> getSudokuCoordinates(Mat input, vector<Point> biggestApprox);

Mat recursiveExtraction(Mat input);
extractionInformation extractPuzzle(Mat input, vector<Point> biggestApprox);
// knn
int readFlippedInteger(FILE *fp);
Ptr<ml::KNearest> getKnn(FileStorage raw_features);
void testKnn(Ptr<ml::KNearest> knn, cv::FileStorage raw_features);
// debug
int minuss(int i, int j);
Mat drawAllContour(Mat preprocessed);
bool isContourValid(vector<Point> contour, vector<Point> approx);
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
template<typename T> static Ptr<T> load_classifier(const string& persistence);
inline TermCriteria TC(int iters, double eps);
static void test_and_save_classifier(const Ptr<StatModel>& model,
                                     const Mat& data, const Mat& responses,
                                     int ntrain_samples, int rdelta,
                                     const string& filename_to_save);

// sudoku
string grabNumbers(Mat image, Ptr<ml::KNearest> knn);
// file system
// std::string getexepath();
string joinPath(vector<string> strs);
vector<string> splitPath(string path);
int isDirectory(const char *path);
int getNumberOfFilesInFolder(string dir);
int getdir(string dir, vector<string> &files);

std::map<int, std::map<int,int> > cellValues();
Mat mouline(Mat original);

// debug for android
string hello();
string askServerForSolution(string initialSudokuState);



#endif //UTILS_OPENCV_LIB


