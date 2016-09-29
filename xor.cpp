#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace cv;
using namespace cv::ml;
using namespace std;

int main(int argc, char **argv) {

    const int hiddenLayerSize = 4;
    float inputTrainingDataArray[4][2] = {
            {0.0, 0.0},
            {0.0, 1.0},
            {1.0, 0.0},
            {1.0, 1.0}
    };
    Mat inputTrainingData = Mat(4, 2, CV_32F, inputTrainingDataArray);

    float outputTrainingDataArray[4][1] = {
            {0.0},
            {1.0},
            {1.0},
            {0.0}
    };
    Mat outputTrainingData = Mat(4, 1, CV_32F, outputTrainingDataArray);

    Ptr<ANN_MLP> mlp = ANN_MLP::create();

    Mat layersSize = Mat(3, 1, CV_16U);
    layersSize.row(0) = Scalar(inputTrainingData.cols);
    layersSize.row(1) = Scalar(hiddenLayerSize);
    layersSize.row(2) = Scalar(outputTrainingData.cols);
    mlp->setLayerSizes(layersSize);

    mlp->setActivationFunction(ANN_MLP::SIGMOID_SYM);

    TermCriteria termCrit = TermCriteria(
            TermCriteria::COUNT + TermCriteria::EPS,
            100000000,
            0.000000000000000001
    );
    mlp->setTermCriteria(termCrit);

    mlp->setTrainMethod(ANN_MLP::BACKPROP);

    Ptr<TrainData> trainingData = TrainData::create(
            inputTrainingData,
            ROW_SAMPLE,
            outputTrainingData
    );

    mlp->train(trainingData
            /*, ANN_MLP::TrainFlags::UPDATE_WEIGHTS
            + ANN_MLP::TrainFlags::NO_INPUT_SCALE
            + ANN_MLP::TrainFlags::NO_OUTPUT_SCALE*/
    );

    for (int i = 0; i < inputTrainingData.rows; i++) {
        Mat sample = Mat(1, inputTrainingData.cols, CV_32F, inputTrainingDataArray[i]);
        Mat result;
        mlp->predict(sample, result);
        cout << sample << " -> " << result << endl;
    }

    return 0;
}
