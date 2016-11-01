/**
 * 1/3
 * This file is not part of the main program.
 *
 * We will iterate in the `puzzles folder` and for each puzzle we will extract all the cells holding a number
 * We will normalize the cell (25x25) and save these images for future use.
 * Future use ?
 * All these images will be used to train the multi layer perceptron artificial neural network
 *
 * Run this program to start classifying the images
 * The result will be a structure like that:
 * data
    ├── 1
    ├── 2
    ├── 3
    ├── 4
    ├── 5
    ├── 6
    ├── 7
    ├── 8
    └── 9
 *
 * where every recognized number (eg 9) will be save in his folder (9)
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * UNRECOGNIZE NUMBER WILL BE SAVE IN THE DATA FOLDER WAITING FOR AN MANUAL ACTION
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * after manual action (the data folder has only folder no more images)
 * Note that in this script we use tesseract to help us classify the cell. Tesseract is not used in the main program
 *
 * run the randomizeDataForTraining.sh script
 *
 * */

#include <opencv2/opencv.hpp>
// #include <tesseract/baseapi.h>
#include "../debug.h"
#include "../utils/picture.h"
#include "../utils/opencv.h"

#include <iostream>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

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
#include <iostream>

namespace fs = boost::filesystem;
using namespace cv;
using namespace std;

std::string remove_extension(const std::string &filename);
std::string uuid_first_part(const std::string &uuid);
char *identifyText(Mat input);
void createPreTrainingData(Mat input, string fileNumber, int cellIndex);
void create_structure();
void createData();
