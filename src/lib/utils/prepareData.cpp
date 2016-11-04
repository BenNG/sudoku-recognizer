#include <iostream>
#include <regex>

#include "opencv.h"

using namespace std;    // Or using std::string;

int main(int argc, char **argv)
{

    Ptr<ml::KNearest> knn(ml::KNearest::create());
    std::map<int, std::map<int,int>> cellV(cellValues());
    int value;
    
    string fullName;
    Mat raw, sudoku;

    fs::path knn_trained_data(getPath("assets/knn-trained-data.yml"));

    if (boost::filesystem::exists(knn_trained_data)) {
        cout << knn_trained_data.string() << endl;
        // knn->load(knn_trained_data.string());

    }else{
        
    }

    int nbrOfCells = 0; // --> 1184

    Mat features(1184, 784, CV_8UC1);
    Mat labels(1, 1184, CV_8UC1);
    Mat roi, normalized;
    for(int i = 0; i < 36 ; i++)    {
        stringstream ss;
        ss << "assets/puzzles/s";
        ss << i;
        ss << ".jpg";
        fs::path p(getPath(ss.str()));

        fullName = p.string();
        // cout << fullName << endl;

        raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
        sudoku = extractPuzzle(raw);

        for (int k = 0; k < 81; k++)
        {
            roi = extractRoiFromCell(sudoku, k);
            if (!roi.empty())
            {
                value = cellV[i][k];
                // cout << k << ":" << value << endl;
                normalized = normalizeSize(roi, 28);
                // featurize
                Mat feat = normalized.reshape(1,1);

                feat.copyTo(features.row(nbrOfCells));
                labels.at<unsigned char>(0,nbrOfCells) = value;

                // showImage(normalized);
                nbrOfCells++;

            }
        }
    }


            features.convertTo(features, CV_32F);
            labels.convertTo(labels, CV_32F);
            knn->train(features, ml::ROW_SAMPLE, labels);
            knn->save(knn_trained_data.string());
            int K = 1;
            Mat response, dist, m;
            knn->findNearest(features.row(1), K, noArray(), response, dist);

            cout << "response: " << response << endl;

        // cout << nbrOfCells << endl;

    return 0;
}
