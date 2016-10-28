#include "mnist.h"

int readFlippedInteger(FILE *fp)
{
    int ret = 0;

    BYTE *temp;

    temp = (BYTE *)(&ret);
    fread(&temp[3], sizeof(BYTE), 1, fp);
    fread(&temp[2], sizeof(BYTE), 1, fp);
    fread(&temp[1], sizeof(BYTE), 1, fp);

    fread(&temp[0], sizeof(BYTE), 1, fp);

    return ret;
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

    Mat trainFeatures(numImages, size, CV_32F), fff;
    Mat trainLabels(1, numImages, CV_32S);

    BYTE *temp = new BYTE[size];
    BYTE tempClass = 0;
    for (int i = 0; i < numImages; i++)
    {
        fread((void *)temp, size, 1, fp);
        fread((void *)(&tempClass), sizeof(BYTE), 1, fp2);

        trainLabels.at<int>(0, i) = (int)tempClass;

        for (int k = 0; k < size; k++)
        {
            trainFeatures.at<float>(i, k) = (float)temp[k];
        }
    }

    v[0] = trainFeatures;
    v[1] = trainLabels;
    return v;
}

vector<Mat> readTrainingMNIST(){
    return readMNIST(true);
}

vector<Mat> readTestMNIST(){
    return readMNIST(false);
}
