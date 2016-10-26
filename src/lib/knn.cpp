#include "knn.h"

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

Ptr<ml::KNearest> getKnn()
{
    Ptr<ml::KNearest> knn(ml::KNearest::create());

    FILE *fp = fopen("/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-images-idx3-ubyte", "rb");
    FILE *fp2 = fopen("/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-labels-idx1-ubyte", "rb");

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

        trainLabels.at<int>(0,i) = (int)tempClass;

        for (int k = 0; k < size; k++)
        {
            trainFeatures.at<float>(i,k) = (float)temp[k];
        }
    }

    knn->train(trainFeatures, ml::ROW_SAMPLE, trainLabels);

    return knn;
}

void testKnn(Ptr<ml::KNearest> knn, bool debug)
{
    int totalCorrect = 0;

    FILE *fp = fopen("/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/t10k-images-idx3-ubyte", "rb");
    FILE *fp2 = fopen("/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/t10k-labels-idx1-ubyte", "rb");

    int magicNumber = readFlippedInteger(fp);
    int numImages = readFlippedInteger(fp);
    int numRows = readFlippedInteger(fp);
    int numCols = readFlippedInteger(fp);
    fseek(fp2, 0x08, SEEK_SET);

    numImages = 100;

    int size = numRows * numCols;

    Mat testFeatures(numImages, size, CV_32F);
    Mat expectedLabels(1, numImages, CV_32S);

    cout << "expectedLabels.type(): " << expectedLabels.type() << endl;

    BYTE *temp = new BYTE[size];
    BYTE tempClass = 0;

    int K = 1;
    Mat response, dist, m;

    for (int i = 0; i < numImages; i++)
    {

        if (i % 1000 == 0 && i != 0)
        {
            cout << i << endl;
        }

        fread((void *)temp, size, 1, fp);
        fread((void *)(&tempClass), sizeof(BYTE), 1, fp2);

        expectedLabels.at<int>(0,i) = (int)tempClass;

        for (int k = 0; k < size; k++)
        {
            testFeatures.at<float>(i,k) = (float)temp[k];
        }

        // test to verify if createMatFromMNIST and createMatToMNIST are well.
        m = testFeatures.row(i);

        knn->findNearest(m, K, noArray(), response, dist);

        if (debug)
        {
            cout << "response: " << response << endl;
            cout << "dist: " << dist << endl;
            Mat m2 = createMatFromMNIST(m);
            showImage(m2);
            Mat m3 = createMatToMNIST(m2);
            showImage(m3);
        }

        if (expectedLabels.at<int>(0,i) == response.at<float>(0))
        {
            totalCorrect++;
        }
    }
    printf("Accuracy: %f ", (double)totalCorrect * 100 / (double)numImages);
}
