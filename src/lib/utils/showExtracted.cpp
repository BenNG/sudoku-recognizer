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


#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

#include "../debug.h"
#include "../mlp.h"
#include "cell.h"
#include "picture.h"


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
    fs::path p(getMyProjectRoot(fs::current_path()));
    string fullName;
    Mat output, raw, preprocessed, sudoku;

    string path_str("assets/puzzles/"); // by default
    if( argc > 1)
    {
        path_str = argv[1];
    }

    p /= path_str;


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
      Mat cell = getCell(sudoku, 71);
      Mat preparedCell = prepareCell(cell);
      Mat roi = extractRoiFromCell(preparedCell);
      Mat normalized = normalizeSize(roi);

      fs::path featured(getMyProjectRoot(fs::current_path()));
      featured /= "assets/featuredDataForTraining.xml";

      fs::path trained_data(getMyProjectRoot(fs::current_path()));
      trained_data /= "assets/trained_data";

      Ptr<ANN_MLP> model = build_mlp_classifier(featured, trained_data);

      Mat feature = features(roi, 15);



      float r = model->predict(feature);
      cout << "r:" << r + 1 << endl;

      showImage(normalized);
    }

    return 0;
}
