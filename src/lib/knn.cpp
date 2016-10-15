// #include <cv.h>
// #include <highgui.h>
// #include <ml.h>
// #include <time.h>

#include <opencv2/opencv.hpp>

#include <stdio.h>

#include <stdlib.h>
#include "debug.h"
#include <opencv2/ml.hpp>
using namespace cv;
using namespace cv::ml;
using namespace std;

typedef unsigned char       BYTE;

int readFlippedInteger(FILE *fp)
{
    int ret = 0;

    BYTE *temp;

    temp = (BYTE*)(&ret);
    fread(&temp[3], sizeof(BYTE), 1, fp);
    fread(&temp[2], sizeof(BYTE), 1, fp);
    fread(&temp[1], sizeof(BYTE), 1, fp);

    fread(&temp[0], sizeof(BYTE), 1, fp);

    return ret;
}

int main()
{

    fs::path trainImages(getMyProjectRoot(fs::current_path()));
    trainImages /= "assets/train-images.idx3-ubyte";

    fs::path trainLabels(getMyProjectRoot(fs::current_path()));
    trainLabels /= "assets/train-labels.idx1-ubyte";

    string trainImages_str = trainImages.string();
    string trainLabels_str = trainLabels.string();

    FILE *fp = fopen(trainImages_str.c_str(), "rb");
    FILE *fp2 = fopen(trainLabels_str.c_str(), "rb");

    if(!fp || !fp2)
           return 0;


    int magicNumber = readFlippedInteger(fp);
    int numImages = readFlippedInteger(fp);
    int numRows = readFlippedInteger(fp);
    int numCols = readFlippedInteger(fp);
    fseek(fp2, 0x08, SEEK_SET);

    int size = numRows*numCols;
    CvMat *trainingVectors = cvCreateMat(numImages, size, CV_32FC1);
    CvMat *trainingLabels = cvCreateMat(numImages, 1, CV_32FC1);


    BYTE *temp = new BYTE[size];
    BYTE tempClass=0;
    for(int i=0;i<numImages;i++)
    {

        fread((void*)temp, size, 1, fp);

        fread((void*)(&tempClass), sizeof(BYTE), 1, fp2);

        trainingLabels->data.fl[i] = tempClass;

        for(int k=0;k<size;k++)
            trainingVectors->data.fl[i*size+k] = temp[k];
    }

    Ptr<TrainData> trainingData;
    Ptr<KNearest> kclassifier=cv::ml::KNearest::create();

    trainingData=TrainData::create(trainingVectors,ROW_SAMPLE,trainingLabels);


    // cv::ml::KNearest knn(trainingVectors, trainingLabels);



    cout << "knn" << endl;
    return 0;
}
