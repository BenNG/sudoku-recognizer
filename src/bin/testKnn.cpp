#include "../lib/hello.h"
#include "../lib/sudoku.h"

int main(int argc, char **argv)
{
    string raw_features_path("./../assets/config/raw-features.yml"); // created by prepareData
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::READ);
    
    Ptr<ml::KNearest> knn = getKnn(raw_features);
    testKnn(knn, raw_features);

    return 0;
}
