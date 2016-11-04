#include <iostream>
#include <regex>

#include "opencv.h"

using namespace std;    // Or using std::string;

int main(int argc, char **argv)
{

    std::map<int, std::map<int,int>> cellV(cellValues());
    int value;
    
    string fullName;
    Mat raw, sudoku;

    for(int i = 0; i < 36 ; i++)    {
    
        stringstream ss;
        ss << "assets/puzzles/s";
        ss << i;
        ss << ".jpg";
        fs::path p(getPath(ss.str()));

        fullName = p.string();
        cout << fullName << endl;

        raw = imread(fullName, CV_LOAD_IMAGE_GRAYSCALE);
        sudoku = extractPuzzle(raw);

        Mat roi, normalized;
        for (int k = 0; k < 81; k++)
        {
            roi = extractRoiFromCell(sudoku, k);
            if (!roi.empty())
            {
                value = cellV[i][k];
                cout << k << ":" << value << endl;
                normalized = normalizeSize(roi, 28);
                showImage(normalized);

            }
        }
    }

    return 0;
}
