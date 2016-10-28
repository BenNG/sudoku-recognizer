#include "knn.h"

Ptr<ml::KNearest> getKnn()
{
    Ptr<ml::KNearest> knn(ml::KNearest::create());

    vector<Mat> v = readTrainingMNIST();
    Mat trainFeatures = v[0];
    Mat trainLabels = v[1];

    knn->train(trainFeatures, ml::ROW_SAMPLE, trainLabels);
    return knn;
}

void testKnn(Ptr<ml::KNearest> knn)
{
    int totalCorrect = 0;

    vector<Mat> v = readTestMNIST();
    Mat testFeatures = v[0];
    Mat expectedLabels = v[1];

    int numImages = testFeatures.rows;

    int K = 1;
    Mat response, dist, m;

    for (int i = 0; i < numImages; i++)
    {
        m = testFeatures.row(i);
        knn->findNearest(m, K, noArray(), response, dist);

        if (expectedLabels.at<int>(0, i) == response.at<float>(0))
            totalCorrect++;
    }

    printf("Accuracy: %f ", (double)totalCorrect * 100 / (double)numImages);
}

void testKnn(Ptr<ml::KNearest> knn, bool debug)
{
    int totalCorrect = 0;

    vector<Mat> v = readTestMNIST();
    Mat testFeatures = v[0];
    Mat expectedLabels = v[1];

    int numImages = testFeatures.rows;

    int K = 1;
    Mat response, dist, m;

    for (int i = 0; i < numImages; i++)
    {

        if (debug && i % 1000 == 0 && i != 0)
        {
            cout << i << endl;
        }

        m = testFeatures.row(i);

        knn->findNearest(m, K, noArray(), response, dist);

        if (debug)
        {
            cout << "response: " << response << endl;
            cout << "dist: " << dist << endl;
            Mat m2 = m.reshape(1, 28);
            showImage(m2);
            Mat m3 = m2.reshape(1, 1);
            showImage(m3);
        }

        if (expectedLabels.at<int>(0, i) == response.at<float>(0))
        {
            totalCorrect++;
        }
    }

    printf("Accuracy: %f ", (double)totalCorrect * 100 / (double)numImages);
}
