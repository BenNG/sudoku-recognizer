/**
 * This module will help you to handle the puzzle
 you can extract a cell with the extractCell function
 * */

#include "puzzle.h"

/**
 * get cell from an image created by the picture fn
 * */
Mat extractCell(Mat sudoku, int numCell) {
    Mat output = sudoku.clone();
    int y = sudoku.cols;
    int x = sudoku.rows;
    int cx = x / 9;
    int cy = y / 9;

//    cout << "cell size is :" << cx << " * " << cy << " = " << cx * cy << endl;

    Rect rect = Rect((numCell % 9) * cy, (numCell / 9) * cx, cy, cx);
    return output(rect);
}
