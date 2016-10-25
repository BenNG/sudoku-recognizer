#include "../hello.h"
#include "../debug.h"
#include "picture.h"
#include "cell.h"
#include "puzzle.h"
#include "../knn.h"

int main(int argc, char **argv)
{
    sayHello("LouLou");


    Ptr<ml::KNearest> knn = getKnn();
    testKnn(knn, false);

    if (fs::is_directory(getPath("assets/puzzles/s0.jpg")))
    {
        cout << "please give a file as a parameter" << endl;
    }else{
        cout << "OK" << endl;
    }

    return 0;
}
