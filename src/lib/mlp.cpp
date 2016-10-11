//
// Created by benng on 06/10/16.
//

#include "mlp.h"


template<typename T>
static Ptr<T> load_classifier(const string& persistence)
{
    // load classifier from the specified file
    Ptr<T> model = StatModel::load<T>( persistence );
    if( model.empty() )
        cout << "Could not read the classifier " << persistence << endl;
    else
        cout << "The classifier " << persistence << " is loaded.\n";

    return model;
}

inline TermCriteria TC(int iters, double eps) {
    return TermCriteria(TermCriteria::MAX_ITER + (eps > 0 ? TermCriteria::EPS : 0), iters, eps);
}


static void test_and_save_classifier(const Ptr<StatModel>& model,
                                     const Mat& data, const Mat& responses,
                                     int ntrain_samples, int rdelta,
                                     const string& filename_to_save)
{
    int i, nsamples_all = data.rows;
    double train_hr = 0, test_hr = 0;

    // compute prediction error on train and test data
    for( i = 0; i < nsamples_all; i++ )
    {
        Mat sample = data.row(i);

        float r = model->predict( sample );
        r = std::abs(r + rdelta - responses.at<int>(i)) <= FLT_EPSILON ? 1.f : 0.f;

        if( i < ntrain_samples )
            train_hr += r;
        else
            test_hr += r;
    }

    test_hr /= nsamples_all - ntrain_samples;
    train_hr = ntrain_samples > 0 ? train_hr/ntrain_samples : 1.;

    printf( "Recognition rate: train = %.1f%%, test = %.1f%%\n",
            train_hr*100., test_hr*100. );

    if( !filename_to_save.empty() )
    {
        model->save( filename_to_save );
    }
}




Ptr<ANN_MLP>
build_mlp_classifier(const string& data_filename, const string& persistence){

    const int class_count = 9;
    Mat data;
    Mat responses;

    cout << boost::filesystem::current_path() << endl;

    // fn
    FileStorage fs;
    fs.open("../assets/featuredDataForTraining.xml", FileStorage::READ);
    fs["TrainingDataF15"] >> data;
    fs["classes"] >> responses;
    // fn - end

    Ptr<ANN_MLP> model;

    int nsamples_all = data.rows;
    int ntrain_samples = (int)(nsamples_all*0.8);

    boost::filesystem::path persistence_path(persistence);
    // Create or load MLP classifier
    if (boost::filesystem::exists(persistence_path)) {
        return load_classifier<ANN_MLP>(persistence);
    }
    else
    {
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //
        // MLP does not support categorical variables by explicitly.
        // So, instead of the output class label, we will use
        // a binary vector of <class_count> components for training and,
        // therefore, MLP will give us a vector of "probabilities" at the
        // prediction stage
        //
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        Mat train_data = data.rowRange(0, ntrain_samples);
        Mat train_responses = Mat::zeros( ntrain_samples, class_count, CV_32F );

        // 1. unroll the responses
//        cout << "Unrolling the responses...\n";
        for (int i = 0; i < ntrain_samples; i++) {
            train_responses.at<float>(i, responses.at<int>(i)) = 1.f;
        }

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
        model->setTermCriteria(TC(max_iter,0));
        model->setTrainMethod(method, method_param);
        model->train(tdata);
        cout << endl;
    }

    test_and_save_classifier(model, data, responses, ntrain_samples, 0, persistence);
    return model;
}