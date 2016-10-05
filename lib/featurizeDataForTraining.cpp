/**
 * 3/3 create data for the classifier
 * this script will create featuredDataForTraining.xml
 *
 * Each cell is represented by a group of features like ProjectedHistogram...
 * Each cell has an associated value (trainingLabels)
 *
 * */
#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <opencv2/opencv.hpp>

#include "cellHandling.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

#include <iostream>
using namespace cv;
using namespace std;

int main(int argc, char **argv) {
    Mat classes;
    Mat trainingDataf5;
    Mat trainingDataf10;
    Mat trainingDataf15;
    Mat trainingDataf20;
    vector<int> trainingLabels;
    boost::filesystem::path randomized("data/randomized");


    unsigned long file_count = 0;
    unsigned long dir_count = 0;
    unsigned long other_count = 0;
    unsigned long err_count = 0;

    if (!boost::filesystem::exists(randomized)) {
        std::cout << "\nNot found: " << randomized << std::endl;
        return 1;
    }

    if (boost::filesystem::is_directory(randomized)) {
        // std::cout << "\nIn directory: " << p << "\n\n";
        boost::filesystem::directory_iterator end_iter;
        for (boost::filesystem::directory_iterator dir_itr(randomized);
             dir_itr != end_iter;
             ++dir_itr) {
            try {
                if (boost::filesystem::is_directory(dir_itr->status())) {
                    ++dir_count;
                    // std::cout << dir_itr->path().filename() << " [directory]\n";
                } else if (boost::filesystem::is_regular_file(dir_itr->status())) {
                    ++file_count;
                    boost::filesystem::path source_path(dir_itr->path());
                    std::cout << source_path << "\n";
                    string source = source_path.string();


                    int lastChar = (int) source.at(source.length() - 5);
                    int value((int) lastChar - 48);
                    int index(value - 1);

                    Mat img = imread(source, 0);
                    Mat f5 = features(img, 5);
                    Mat f10 = features(img, 10);
                    Mat f15 = features(img, 15);
                    Mat f20 = features(img, 20);


                    trainingDataf5.push_back(f5);
                    trainingDataf10.push_back(f10);
                    trainingDataf15.push_back(f15);
                    trainingDataf20.push_back(f20);
                    trainingLabels.push_back(index);


                } else {
                    ++other_count;
                    // std::cout << dir_itr->path().filename() << " [other]\n";
                }

            }
            catch (const std::exception &ex) {
                ++err_count;
                std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
            }
        }
//        std::cout << "\n" << file_count << " files\n"
//                  << dir_count << " directories\n"
//                  << other_count << " others\n"
//                  << err_count << " errors\n";
    } else // must be a file
    {
//        std::cout << "\nFound: " << p << "\n";
    }


    trainingDataf5.convertTo(trainingDataf5, CV_32F);
    trainingDataf10.convertTo(trainingDataf10, CV_32F);
    trainingDataf15.convertTo(trainingDataf15, CV_32F);
    trainingDataf20.convertTo(trainingDataf20, CV_32F);
    Mat(trainingLabels).copyTo(classes);

    FileStorage fs("featuredDataForTraining.xml", FileStorage::WRITE);
    fs << "TrainingDataF5" << trainingDataf5;
    fs << "TrainingDataF10" << trainingDataf10;
    fs << "TrainingDataF15" << trainingDataf15;
    fs << "TrainingDataF20" << trainingDataf20;
    fs << "classes" << classes;
    fs.release();

    return 0;
}
