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
            Mat cell = extractCell(sudoku, k);
            Mat roi = extractNumber(cell);

            if (!roi.empty())
            {
                adaptiveThreshold(roi, fin, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);
                // fin 8bits (CV_8U)
                fin2 = removeTinyVolume(fin, 90, Scalar(0, 0, 0));
                // fin2 8bits
                vector<double> v = findBiggestComponent(fin2);

                double left = v[0];
                double top = v[1];
                double width = v[2];
                double height = v[3];
                double x = v[4];
                double y = v[5];
                Rect rect(left, top, width, height);
                fin3 = fin2(rect);

                cout << "fin3.type()" << fin3.type() << endl;

                Mat normalized = normalizeSize(fin3), dest;

                threshold(normalized, dest, 100, 255, normalized.type());

                int notZero = 0;
                int sumI = 0, sumY = 0;
                for (int i = 0; i < 20; i++)
                {
                    for (int j = 0; j < 20; j++)
                    {
                        if (dest.at<float>(i, j) != 0)
                        {
                            sumI = sumI + i;
                            sumY = sumY + j;
                            notZero++;
                        }
                    }
                }

                int size = 28;
                int mid = size / 2;

                Mat output = Mat::zeros(size, size, CV_32F);
                normalized.copyTo(output(Rect((mid - sumI / (double)notZero), (mid - sumY / (double)notZero), normalized.cols, normalized.rows)));

                output.convertTo(output,CV_32F);

                knn->findNearest(output.reshape(1,1), K, noArray(), response, dist);
                cout << "response: " << response << endl;
                cout << "dist: " << dist << endl;
                showImage(output);
            }
        }
    }
    return 0;
}
