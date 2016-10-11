#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>
#include "../src/lib/add.h"
#include "../src/lib/debug.h"
#include "../src/lib/utils/sudoku.h"
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

BOOST_AUTO_TEST_CASE(universeInOrder)
{
    BOOST_CHECK(add(2, 2) == 4);
};
BOOST_AUTO_TEST_CASE(universeInOrder2)
{
    BOOST_CHECK(minuss(2, 2) == 0);
};
BOOST_AUTO_TEST_CASE(sudoku_s0)
{
    BOOST_CHECK(grab("../../assets/puzzles/s0.jpg") == "eza");
};