#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "lib/debug.h"
#include "lib/extractPuzzle.h"
#include "lib/cellHandling.h"
#include "opencv2/text.hpp"

using namespace cv;
using namespace cv::ml;
using namespace std;

Scalar white(255, 255, 255);
Scalar green(0, 235, 0);
Scalar red(255, 0, 0);
Scalar blue(0, 0, 255);

Scalar randomColor(RNG &rng) {
    int icolor = (unsigned) rng;
    return Scalar(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);
}

Ptr<ANN_MLP> mlp;
const int hiddenLayerSize = 3;


const char strCharacters[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
const int numCharacters = 9;
const int numFilesChars[] = {11, 14, 15, 13, 13, 18, 18, 15, 9};


void createDataForTraining() {

    char *path = "./training/";

    Mat classes;
    Mat trainingDataf5;
    Mat trainingDataf10;
    Mat trainingDataf15;
    Mat trainingDataf20;

    vector<int> trainingLabels;
    for (int i = 0; i < numCharacters; i++) {
        int numFiles = numFilesChars[i];
        for (int j = 1; j <= numFiles; j++) {

            stringstream ss(stringstream::in | stringstream::out);
            ss << path << strCharacters[i] << "/" << j << ".jpg";

            Mat img = imread(ss.str(), 0);
            Mat f5 = features(img, 5);
            Mat f10 = features(img, 10);
            Mat f15 = features(img, 15);
            Mat f20 = features(img, 20);

            trainingDataf5.push_back(f5);
            trainingDataf10.push_back(f10);
            trainingDataf15.push_back(f15);
            trainingDataf20.push_back(f20);
            trainingLabels.push_back(i);
        }
    }

    trainingDataf5.convertTo(trainingDataf5, CV_32F);
    trainingDataf10.convertTo(trainingDataf10, CV_32F);
    trainingDataf15.convertTo(trainingDataf15, CV_32F);
    trainingDataf20.convertTo(trainingDataf20, CV_32F);
    Mat(trainingLabels).copyTo(classes);

    cout << classes << endl;

    FileStorage fs("OCR.xml", FileStorage::WRITE);
    fs << "TrainingDataF5" << trainingDataf5;
    fs << "TrainingDataF10" << trainingDataf10;
    fs << "TrainingDataF15" << trainingDataf15;
    fs << "TrainingDataF20" << trainingDataf20;
    fs << "classes" << classes;
    fs.release();

}

static bool
read_num_class_data( const string& filename, int var_count,
                     Mat* _data, Mat* _responses )
{
    const int M = 1024;
    char buf[M+2];

    Mat el_ptr(1, var_count, CV_32F);
    int i;
    vector<int> responses;

    _data->release();
    _responses->release();

    FILE* f = fopen( filename.c_str(), "rt" );
    if( !f )
    {
        cout << "Could not read the database " << filename << endl;
        return false;
    }

    for(;;)
    {
        char* ptr;
        if( !fgets( buf, M, f ) || !strchr( buf, ',' ) )
            break;
        responses.push_back((int)buf[0]);
        ptr = buf+2;
        for( i = 0; i < var_count; i++ )
        {
            int n = 0;
            sscanf( ptr, "%f%n", &el_ptr.at<float>(i), &n );
            ptr += n + 1;
        }
        if( i < var_count )
            break;
        _data->push_back(el_ptr);
    }
    fclose(f);
    Mat(responses).copyTo(*_responses);

    cout << "The database " << filename << " is loaded.\n";

    return true;
}

inline TermCriteria TC(int iters, double eps)
{
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

    char *path = "./training/";

    Mat classes;
    Mat trainingDataf5;
    Mat trainingDataf10;
    Mat trainingDataf15;
    Mat trainingDataf20;

    vector<int> trainingLabels;
    for (int i = 0; i < numCharacters; i++) {
        int numFiles = numFilesChars[i];
        for (int j = 1; j <= numFiles; j++) {

            stringstream ss(stringstream::in | stringstream::out);
            ss << path << strCharacters[i] << "/" << j << ".jpg";

            Mat img = imread(ss.str(), 0);
            Mat f5 = features(img, 5);
            Mat f10 = features(img, 10);
            Mat f15 = features(img, 15);
            Mat f20 = features(img, 20);

            trainingDataf5.push_back(f5);
            trainingDataf10.push_back(f10);
            trainingDataf15.push_back(f15);
            trainingDataf20.push_back(f20);
            trainingLabels.push_back(i);
        }
    }

    trainingDataf5.convertTo(trainingDataf5, CV_32F);
    trainingDataf10.convertTo(trainingDataf10, CV_32F);
    trainingDataf15.convertTo(trainingDataf15, CV_32F);
    trainingDataf20.convertTo(trainingDataf20, CV_32F);
    Mat(trainingLabels).copyTo(classes);


    const int class_count = 9;
    data = trainingDataf15;
    responses = classes;

    Ptr<ANN_MLP> model;

    int nsamples_all = data.rows;
    int ntrain_samples = (int)(nsamples_all*1);

    Mat train_data = data.rowRange(0, ntrain_samples);
    Mat train_responses = Mat::zeros( ntrain_samples, class_count, CV_32F );

    // 1. unroll the responses
    cout << "Unrolling the responses...\n";
    for( int i = 0; i < ntrain_samples; i++ )
    {
        train_responses.at<float>(i, responses.at<int>(i)) = 1.f;
    }

        cout << train_responses << endl;

    // 2. train classifier
    int layer_sz[] = { data.cols, 100, 100, class_count };
    int nlayers = (int)(sizeof(layer_sz)/sizeof(layer_sz[0]));
    Mat layer_sizes( 1, nlayers, CV_32S, layer_sz );

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
    model->setTermCriteria(TC(max_iter,0));
    model->setTrainMethod(method, method_param);
    model->train(tdata);
    cout << endl;

//    static void test_and_save_classifier(const Ptr<StatModel>& model, const Mat& data, const Mat& responses, int ntrain_samples, int rdelta, const string& filename_to_save)
//    test_and_save_classifier(model, data, responses, ntrain_samples, 'A', filename_to_save);


    Mat sample = data.row(125);

    float r = model->predict( sample );

    cout << "r:" << r << endl;


    const char *files[] = {
            "../puzzles/s0.jpg",
            "../puzzles/s1.jpg",
            "../puzzles/s2.jpg",
            "../puzzles/s3.jpg",
    };

    unsigned nb_files = sizeof(files) / sizeof(const char *);

    for (unsigned j = 0; j < nb_files; ++j) {
        int error = 0;
        Mat raw = imread(files[j], CV_LOAD_IMAGE_GRAYSCALE);
        Mat preprocessed = preprocess(raw.clone());
        vector<Point> biggestApprox = findBigestApprox(preprocessed);

        Mat sudoku = extractPuzzle(raw, biggestApprox);

//        read_num_class_data("../data/letter-recognition.data", 16, &data, &responses);


//        cout << "data" << endl;
//        cout << data.rows << endl;
//        cout << data.row(0).cols << endl;
//
//        cout << "responses" << endl;
//        cout << responses.rows << endl;
//        cout << responses.row(0).cols << endl;

//        cout << responses << endl;





//        for (unsigned i = 0; i < 81; i++) {
//            Mat cell = getCell(sudoku, i), cell_no_noise, cell_no_light, final_cell;
//
//            // remove noise
//            medianBlur(cell, cell_no_noise, 1);
//            // remove background/light
//            cell_no_light = removeLight(cell_no_noise, calculateLightPattern(cell), 2);
//            // binarize image
//            adaptiveThreshold(cell_no_light, final_cell, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);
//
//            Mat roi = extractRoiFromCell(final_cell);
//
//
//
////
////            if (roi.empty()) {
////
////            } else {
//////                showImage(roi);
////
////
////            }
////
//
//
//
//        }







    }

    return 0;
}
