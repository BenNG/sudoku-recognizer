#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

namespace fs = boost::filesystem;

#include "../hello.h"
#include "../debug.h"
#include "picture.h"
#include "cell.h"
#include "puzzle.h"
#include "../knn.h"

int main(int argc, char **argv)
{
    sayHello("LouLou");

    int K = 1;
    Mat response, dist, fin, fin2, fin3;
    string path_str("assets/puzzles/s0.jpg"); // by default
    if (argc > 1)
    {
        path_str = argv[1];
    }
    fs::path p(getPath(path_str));

    if (fs::is_directory(p))
    {
        cout << "please give a file as a parameter" << endl;
    }
    else
    {
        Ptr<ml::KNearest> knn = getKnn();
        string fullName = p.string();
        Mat raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
        Mat sudoku = extractPuzzle(raw);

        for (int k = 0; k < 81; k++)
        {
            // cout << "index: " << k << endl;
            Mat cell = extractCell(sudoku, k);
            // Mat preparedCell = prepareCell(cell);
            Mat roi = extractNumber(cell);

            if (!roi.empty())
            {
                adaptiveThreshold(roi, fin, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);
                fin2 = removeTinyVolume(fin, 90, Scalar(0, 0, 0));
                fin3 = findBiggestComponent(fin2);

                Mat normalized = normalizeSize(fin3);
                // showImage(normalized);
                //
                int size = 28;
                int mid = size / 2;
                //
                Mat output = Mat::zeros(size, size, normalized.type());
                normalized.copyTo(output(Rect((mid - normalized.rows / 2), (mid - normalized.cols / 2), normalized.cols, normalized.rows)));
                // //
                // // showImage(output);
                showImage(output);
                Mat test = createMatToMNIST(output);

                knn->findNearest(test, K, noArray(), response, dist);
                cout << "response: " << response << endl;
                cout << "dist: " << dist << endl;
                showImage(output);
            }
        }
        // showImage(sudoku);
    }
    return 0;
}
