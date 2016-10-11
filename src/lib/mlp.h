#include <opencv2/opencv.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
using namespace cv;
using namespace cv::ml;
using namespace std;
namespace fs = boost::filesystem;


#ifndef DISPLAYIMAGE_MLP_H
#define DISPLAYIMAGE_MLP_H

template<typename T> static Ptr<T> load_classifier(const string& persistence);
inline TermCriteria TC(int iters, double eps);
static void test_and_save_classifier(const Ptr<StatModel>& model,
                                     const Mat& data, const Mat& responses,
                                     int ntrain_samples, int rdelta,
                                     const string& filename_to_save);

Ptr<ANN_MLP> build_mlp_classifier(const fs::path data_filename, const string& persistence);

#endif //DISPLAYIMAGE_MLP_H
