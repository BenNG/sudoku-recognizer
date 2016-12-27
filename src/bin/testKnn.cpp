#include "../lib/hello.h"
#include "../lib/sudoku.h"

int main(int argc, char **argv)
{
    string raw_features_path(getPath("assets/raw-features.yml")); // created by prepareData
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::READ);
    
    Ptr<ml::KNearest> knn = getKnn(raw_features);
    testKnn(knn, raw_features);


    if (isDirectory(getPath("assets/puzzles/s0.jpg").c_str()))
    {
        cout << "please give a file as a parameter" << endl;
    }else{
        cout << "OK" << endl;
    }

    return 0;
}
