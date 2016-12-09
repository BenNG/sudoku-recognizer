#include "../hello.h"
#include "opencv.h"

int main(int argc, char **argv)
{
    Ptr<ml::KNearest> knn = getKnn();
    testKnn(knn);


    if (isDirectory(getPath("assets/puzzles/s0.jpg").c_str()))
    {
        cout << "please give a file as a parameter" << endl;
    }else{
        cout << "OK" << endl;
    }

    return 0;
}
