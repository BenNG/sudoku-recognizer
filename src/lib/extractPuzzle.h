#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*
 * preprocess
 *
 * It's going to be easier for the opencv algorithm to detect the border of the sudoku
 * There is a Threshold where everything is white or black
 */
Mat preprocess(Mat input);

/*
 * findBigestApprox
 *
 * Find the biggest contour in the image
 * note that it returns vector< vector<Point> > because it is more convenient to use drawContours after
 * */
vector<Point> findBigestApprox(Mat input);

Mat extractPuzzle(Mat input, vector<Point> biggestApprox);
