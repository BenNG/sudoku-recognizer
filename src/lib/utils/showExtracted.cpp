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

    string path_str("assets/puzzles/"); // by default
    if( argc > 1)
    {
        path_str = argv[1];
    }

    Ptr<ml::KNearest> knn = getKnn();

    // FILE *fp = fopen("/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-images-idx3-ubyte", "rb");
    // FILE *fp2 = fopen("/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-labels-idx1-ubyte", "rb");
    //
    // if(!fp || !fp2){
    //   cout << "can't open file" << endl;
    //   return 0;
    // }
    //
    // int magicNumber = readFlippedInteger(fp);
    // int numImages = readFlippedInteger(fp);
    // int numRows = readFlippedInteger(fp);
    // int numCols = readFlippedInteger(fp);
    // fseek(fp2, 0x08, SEEK_SET);
    //
    // int size = numRows*numCols;
    //
    // cout << "size: " << size << endl;
    // cout << "rows: " << numRows << endl;
    // cout << "cols: " << numCols << endl;
    //
    // Mat_<float> trainFeatures(numImages, size);
    // Mat_<int> trainLabels(1,numImages);
    //
    // BYTE *temp = new BYTE[size];
    // BYTE tempClass=0;
    // for(int i=0;i<numImages;i++)
    // {
    //     fread((void*)temp, size, 1, fp);
    //     fread((void*)(&tempClass), sizeof(BYTE), 1, fp2);
    //
    //     trainLabels[0][i] = (int)tempClass;
    //
    //     for(int k=0;k<size;k++){
    //         trainFeatures[i][k] = (float)temp[k];
    //     }
    // }
    //
    // Mat_<float> test(numRows, numCols);
    //
    // // Mat_<float> testFeature(1, size);
    // int expected_resp = trainLabels[0][5];
    //
    // Mat_<float> testFeature = trainFeatures.row(5);
    // Mat_<float> testImage = createMatFromMNIST( trainFeatures.row(5) );
    //
    // showImage(testImage);
    //
    // Ptr<ml::KNearest>  knn(ml::KNearest::create());
    // knn->train(trainFeatures, ml::ROW_SAMPLE, trainLabels);
    //
    // int K=1;
    // Mat response,dist;
    // knn->findNearest(testFeature, K, noArray(), response, dist);
    // cerr << response << endl;
    // cerr << dist<< endl;
    //
    // cout << "expected: " << expected_resp << endl;

    // int K=1;
    // Mat response,dist;
    // knn->findNearest(trainFeatures[5], K, noArray(), response, dist);
    // cout << trainLabels[0][5] << endl;
    //
    // cerr << response << endl;
    // cerr << dist<< endl;

    // fs::path p(getPath(path_str));
    //
    // if (fs::is_directory(p)) {
    //     fs::directory_iterator end_iter;
    //     for (fs::directory_iterator dir_itr(p);
    //          dir_itr != end_iter;
    //          ++dir_itr) {
    //                 fullName = dir_itr->path().string();
    //                 cout << fullName << endl;
    //                 raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
    //                 sudoku = extractPuzzle(raw);
    //                 showImage(sudoku);
    //     }
    // }else{
    //   fullName = p.string();
    //   raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
    //   // preprocessed = preprocess(raw.clone());
    //   sudoku = extractPuzzle(raw);
    //
    //
    //   Mat preprocessed = preprocess(raw.clone());
    //   // vector<Point> biggestApprox = findBigestApprox(preprocessed.clone());
    //   // if(biggestApprox.empty()){
    //   //   cout << "no approx found" << endl;
    //   // }
    //
    //   // vector<Point> biggestApprox = findBigestApprox(preprocessed);
    //   // sudoku = removeTinyVolume(preprocessed, raw);
    //   // sudoku = drawAllContour(preprocessed, raw);
    //   // sudoku = drawBiggestContour(preprocessed, raw);
    //   // sudoku = drawAllApprox(preprocessed, raw);
    //   Mat cell = extractCell(sudoku, 71);
    //   Mat preparedCell = prepareCell(cell);
    //   Mat roi = extractNumber(preparedCell);
    //   Mat normalized = normalizeSize(roi);
    //
    //   fs::path featured(getPath("assets/featuredDataForTraining.xml"));
    //   fs::path trained_data(getPath("assets/trained_data"));
    //
    //   Ptr<ANN_MLP> model = build_mlp_classifier(featured, trained_data);
    //
    //   Mat feature = features(roi, 15);
    //
    //
    //
    //   float r = model->predict(feature);
    //   cout << "r:" << r + 1 << endl;
    //
    //   showImage(normalized);
    // }

    return 0;
}
