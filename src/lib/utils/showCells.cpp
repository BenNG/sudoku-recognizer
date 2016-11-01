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
#include "picture.h"
#include "cell.h"
#include "puzzle.h"
#include "../knn.h"

int main(int argc, char **argv)
{
    sayHello("LouLou");

    int K = 1;
    Mat response, dist, fin, fin2, roi;
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
        // testKnn(knn, true);
        string fullName = p.string();
        Mat raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
        Mat sudoku = extractPuzzle(raw);

        for (int k = 0; k < 81; k++)
        {
            Mat roi = extractRoiFromCell(sudoku, k);

            if (!roi.empty())
            {

                // cout << "roi.type()" << roi.type() << endl;

                // Mat normalized = normalizeSize(roi), dest;

                // threshold(normalized, dest, 100, 255, normalized.type());

                // int notZero = 0;
                // int sumI = 0, sumY = 0;
                // for (int i = 0; i < 20; i++)
                // {
                //     for (int j = 0; j < 20; j++)
                //     {
                //         if (dest.at<float>(i, j) != 0)
                //         {
                //             sumI = sumI + i;
                //             sumY = sumY + j;
                //             notZero++;
                //         }
                //     }
                // }

                int size = 32;
                int mid = size / 2;

                Mat output = Mat::zeros(size, size, CV_8UC1);
                // normalized.copyTo(output(Rect((mid - sumI / (double)notZero), (mid - sumY / (double)notZero), normalized.cols, normalized.rows)));
                roi.copyTo(output(Rect(mid - roi.cols / 2, mid - roi.rows / 2, roi.cols, roi.rows)));
                Mat normalized = normalizeSize(roi, 28);
                normalized.convertTo(normalized, CV_32F);

                // output = deskew(output);
                // showImage(normalized);

                knn->findNearest(normalized.reshape(1, 1), K, noArray(), response, dist);
                cout << "response: " << response << endl;
                cout << "dist: " << dist << endl;
                showImage(normalized);
            }
            else
            {
                cout << "O" << endl;
            }
        }
    }
    return 0;
}
