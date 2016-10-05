#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include "opencv2/text.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/progress.hpp"

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "lib/debug.h"
#include "lib/extractPuzzle.h"
#include "lib/cellHandling.h"

using namespace cv;
using namespace cv::ml;
using namespace std;
using namespace boost;


Scalar white(255, 255, 255);
Scalar green(0, 235, 0);
Scalar red(255, 0, 0);
Scalar blue(0, 0, 255);

Scalar randomColor(RNG &rng) {
    int icolor = (unsigned) rng;
    return Scalar(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);
}

Ptr<ANN_MLP> model;
const int hiddenLayerSize = 3;

static bool
read_num_class_data(const string &filename, int var_count,
                    Mat *_data, Mat *_responses) {
    const int M = 1024;
    char buf[M + 2];

    Mat el_ptr(1, var_count, CV_32F);
    int i;
    vector<int> responses;

    _data->release();
    _responses->release();

    FILE *f = fopen(filename.c_str(), "rt");
    if (!f) {
        cout << "Could not read the database " << filename << endl;
        return false;
    }

    for (;;) {
        char *ptr;
        if (!fgets(buf, M, f) || !strchr(buf, ','))
            break;
        responses.push_back((int) buf[0]);
        ptr = buf + 2;
        for (i = 0; i < var_count; i++) {
            int n = 0;
            sscanf(ptr, "%f%n", &el_ptr.at<float>(i), &n);
            ptr += n + 1;
        }
        if (i < var_count)
            break;
        _data->push_back(el_ptr);
    }
    fclose(f);
    Mat(responses).copyTo(*_responses);

    cout << "The database " << filename << " is loaded.\n";

    return true;
}

inline TermCriteria TC(int iters, double eps) {
    return TermCriteria(TermCriteria::MAX_ITER + (eps > 0 ? TermCriteria::EPS : 0), iters, eps);
}


/**
 *
 * ----------------------------- Main ------------------------------------
 *
 * */
int main(int argc, char **argv) {
    Mat data;
    Mat responses;
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


    const int class_count = 9;
    data = trainingDataf15;
    responses = classes;

    int nsamples_all = data.rows;
    int ntrain_samples = (int) (nsamples_all * 0.8);

    Mat train_data = data.rowRange(0, ntrain_samples);
    Mat train_responses = Mat::zeros(ntrain_samples, class_count, CV_32F);

    // 1. unroll the responses
    cout << "Unrolling the responses...\n";
    for (int i = 0; i < ntrain_samples; i++) {
        train_responses.at<float>(i, responses.at<int>(i)) = 1.f;
    }


    cout << train_responses << endl;

    // 2. train classifier
    int layer_sz[] = {data.cols, 100, 100, class_count};
    int nlayers = (int) (sizeof(layer_sz) / sizeof(layer_sz[0]));
    Mat layer_sizes(1, nlayers, CV_32S, layer_sz);

#if 1
    int method = ANN_MLP::BACKPROP;
    double method_param = 0.001;
    int max_iter = 300;
#else
    int method = ANN_MLP::RPROP;
        double method_param = 0.1;
        int max_iter = 1000;
#endif

    Ptr<TrainData> tdata = TrainData::create(train_data, ROW_SAMPLE, train_responses);

    cout << "Training the classifier (may take a few minutes)...\n";
    model = ANN_MLP::create();
    model->setLayerSizes(layer_sizes);
    model->setActivationFunction(ANN_MLP::SIGMOID_SYM, 0, 0);
    model->setTermCriteria(TC(max_iter, 0));
    model->setTrainMethod(method, method_param);
    model->train(tdata);
    cout << endl;


    Mat sample = data.row(100);

    float r = model->predict(sample);

    cout << "r:" << r << endl;

    return 0;
}
