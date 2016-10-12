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

bool compare(string actual, string expected){
    stringstream a;
    bool res = true;
    int length = actual.length();
    for(int i = 0; i < length; i++){
//        cout << actual[i] << endl;
//        cout << expected[i] << endl;
        if(actual[i] != expected[i]){
            a << "Expected:";
            a << expected[i];
            a << " but got:";
            a << actual[i];
            a << " at index:";
            a << i;
            cout << a.str() << endl;
            res = false;
            break;
        }
    }
    return res;
}

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
    const string actual = grab("assets/puzzles/s0.jpg");
    cout << actual << endl;
    const string expected = "000604700706000009000005080070020093800000005430010070050200000300000208002301000";
    BOOST_CHECK( compare(actual, expected) == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s1)
{
    const string actual = grab("assets/puzzles/s1.jpg");
    cout << actual << endl;
    const string expected = "800004200420070800007500001070000652002000300683000010100006700006050093008300004";
    BOOST_CHECK( compare(actual, expected) == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s2)
{
    const string actual = grab("assets/puzzles/s2.jpg");
    cout << actual << endl;
    const string expected = "007620058014090006006003002008300705070000040309004600700400100100080260890067500";
    BOOST_CHECK( compare(actual, expected) == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s3)
{
    const string actual = grab("assets/puzzles/s3.jpg");
    cout << actual << endl;
    const string expected = "007620058014090006006003002008300705070000040309004600700400100100080260890067500";
    BOOST_CHECK( compare(actual, expected) == true);
};
