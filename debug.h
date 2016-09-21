//#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void drawAllContour(Mat input, Mat output);
void drawAllApprox(Mat input, Mat output);
void drawMarkers(Mat input, vector<Point> biggestApprox);
Mat drawGrid(Mat input);
void showImage(Mat img);
