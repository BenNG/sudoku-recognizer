#include "mnist.h"

int readFlippedInteger(FILE *fp)
{
    int ret = 0;

    unsigned char *temp;

    temp = (unsigned char *)(&ret);
    fread(&temp[3], sizeof(unsigned char), 1, fp);
    fread(&temp[2], sizeof(unsigned char), 1, fp);
    fread(&temp[1], sizeof(unsigned char), 1, fp);

    fread(&temp[0], sizeof(unsigned char), 1, fp);

    return ret;
}

vector<Mat> readTrainingMNIST(){
    return readMNIST(true);
}

vector<Mat> readTestMNIST(){
    return readMNIST(false);
}

vector<Mat> readMNIST(bool training)
{
    vector<Mat> v(2);
    string feat_str, label_str; 
    if(training){
        feat_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-images-idx3-ubyte";
        label_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-labels-idx1-ubyte";
    }else{
        feat_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/t10k-images-idx3-ubyte";
        label_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/t10k-labels-idx1-ubyte";
    }


    FILE *fp = fopen(feat_str.c_str(), "rb");
    FILE *fp2 = fopen(label_str.c_str(), "rb");

    if (!fp || !fp2)
    {
        cout << "can't open file" << endl;
    }

    int magicNumber = readFlippedInteger(fp);
    int numImages = readFlippedInteger(fp);
    int numRows = readFlippedInteger(fp);
    int numCols = readFlippedInteger(fp);
    fseek(fp2, 0x08, SEEK_SET);

    int size = numRows * numCols;

    cout << "size: " << size << endl;
    cout << "rows: " << numRows << endl;
    cout << "cols: " << numCols << endl;

    Mat trainFeatures(numImages, size, CV_8UC1);
    Mat trainLabels(1, numImages, CV_8UC1);

    unsigned char *temp = new unsigned char[size];
    unsigned char tempClass = 0;
    for (int i = 0; i < numImages; i++)
    {
        fread((void *)temp, size, 1, fp);
        fread((void *)(&tempClass), sizeof(unsigned char), 1, fp2);

        trainLabels.at<unsigned char>(0, i) = tempClass;
        // feature creation
        for (int k = 0; k < size; k++)
        {
            trainFeatures.at<unsigned char>(i, k) = temp[k];
        }
        // feature creation - end
    }

    // I don't know why I have to convert the matrices in float ...
    trainFeatures.convertTo(trainFeatures, CV_32F);
    trainLabels.convertTo(trainLabels, CV_32F);

    v[0] = trainFeatures;
    v[1] = trainLabels;
    return v;
}