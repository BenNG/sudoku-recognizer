/**
this file show the extrated sudoku for each file
usage:

Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted

Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted assets/puzzles/s15.jpg

*/

#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

typedef unsigned char       BYTE;

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

#include "../hello.h"
#include "../knn.h"
#include "../debug.h"
#include "../mlp.h"
#include "puzzle.h"
#include "picture.h"
#include "cell.h"


#include <opencv2/opencv.hpp>

namespace fs = boost::filesystem;
using namespace cv;
using namespace cv::ml;
using namespace std;
using namespace boost;

/**
 *
 * ----------------------------- Main ------------------------------------
 *
 * */
int main(int argc, char **argv) {

    sayHello("LouLou");

    string fullName;
    Mat output, raw, preprocessed, sudoku;


    int K=1;
    Mat response,dist;


    string path_str("assets/puzzles/"); // by default
    if( argc > 1)
    {
        path_str = argv[1];
    }

    fs::path p(getPath(path_str));

       if (fs::is_directory(p)) {
           fs::directory_iterator end_iter;
           for (fs::directory_iterator dir_itr(p);
                dir_itr != end_iter;
                ++dir_itr) {
                       fullName = dir_itr->path().string();
                       cout << fullName << endl;

                       raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);

                       sudoku = extractPuzzle(raw);

                       // sometimes the biggest area found is not correct, our puzzle is inside the extract image
                       // so we do it a second time to extract the biggest blob which is this time our puzzle
                       // this is the case for s6.jpg and s9.jpg for example

                       // Mat preprocessed2 = preprocess(sudoku.clone());
                       // vector<Point> biggestApprox2 = findBigestApprox(preprocessed2);
                       //
                       // if(!biggestApprox2.empty()){
                       //   sudoku = extractPuzzle(sudoku, biggestApprox2);
                       // }

                       // Mat sudoku2 = extractPuzzle(sudoku, biggestApprox2);
                       // output = drawAllApprox(preprocessed, raw);

                       showImage(sudoku);

           }
       }else{
         fullName = p.string();
         raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
         // preprocessed = preprocess(raw.clone());
         sudoku = extractPuzzle(raw);


         Mat preprocessed = preprocess(raw.clone());
         // vector<Point> biggestApprox = findBigestApprox(preprocessed.clone());
         // if(biggestApprox.empty()){
         //   cout << "no approx found" << endl;
         // }

         // vector<Point> biggestApprox = findBigestApprox(preprocessed);
         // sudoku = removeTinyVolume(preprocessed, raw);
         // sudoku = drawAllContour(preprocessed, raw);
         // sudoku = drawBiggestContour(preprocessed, raw);
         // sudoku = drawAllApprox(preprocessed, raw);

         for(int k = 0; k<81 ; k++){

                    Mat cell = extractCell(sudoku, k);
                    Mat preparedCell = prepareCell(cell);
                    Mat roi = extractNumber(preparedCell);
                    Mat normalized = normalizeSize(roi);

                    int size = 28;
                    int mid = size / 2;

                    Mat output = Mat::zeros(size, size, normalized.type());

                    normalized.copyTo(output(Rect((mid - normalized.rows / 2),(mid - normalized.cols / 2),normalized.cols, normalized.rows)));

                    Mat reshape = output.clone().reshape(1, 1);

                    int x = (mid - output.rows / 2);
                    int y = (mid - output.cols / 2);

                    cout << reshape.size() << endl;
                    cout << x << endl;
                    cout << y << endl;

                     Ptr<ml::KNearest> knn = getKnn();
                     // testKnn(knn);
                     //
                     int numCols = 28;
                     int numRows = 28;

                     Mat_<float> test(1, numCols * numRows);

                     for(int i=0;i<numRows;i++)
                     {
                         for(int j=0;j<numCols;j++)
                         {
                           // cout << i * numCols + j << endl;
                           test[0][i*numCols + j] = normalized.at<float>(i,j);
                         }
                     }
                     //
                    //  knn->findNearest(test, K, noArray(), response, dist);
                    //  cout << response << endl;
                     showImage(normalized);
         }

       }

    return 0;
}
