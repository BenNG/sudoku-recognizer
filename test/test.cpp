#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>
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

// BOOST_AUTO_TEST_CASE(universeInOrder)
// {
//     BOOST_CHECK(add(2, 2) == 4);
// };
// BOOST_AUTO_TEST_CASE(universeInOrder2)
// {
//     BOOST_CHECK(minuss(2, 2) == 0);
// };
// BOOST_AUTO_TEST_CASE(sudoku_s0)
// {
//     const string actual = grab("assets/puzzles/s0.jpg");
//     cout << actual << endl;
//     const string expected = "000604700706000009000005080070020093800000005430010070050200000300000208002301000";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };

// BOOST_AUTO_TEST_CASE(sudoku_s1)
// {
//     const string actual = grab("assets/puzzles/s1.jpg");
//     cout << actual << endl;
//     const string expected = "800004200420070800007500001070000652002000300683000010100006700006050093008300004";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };

// BOOST_AUTO_TEST_CASE(sudoku_s2)
// {
//     const string actual = grab("assets/puzzles/s2.jpg");
//     cout << actual << endl;
//     const string expected = "007620058014090006006003002008300705070000040309004600700400100100080260890067500";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };

// BOOST_AUTO_TEST_CASE(sudoku_s3)
// {
//     const string actual = grab("assets/puzzles/s3.jpg");
//     cout << actual << endl;
//     const string expected = "007620058014090006006003002008300705070000040309004600700400100100080260890067500";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };

// BOOST_AUTO_TEST_CASE(sudoku_s4)
// {
//     const string actual = grab("assets/puzzles/s4.jpg");
//     cout << actual << endl;
//     const string expected = "020093000079000841050001000003050408005806300108030500000100080514000920000340050";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };

// BOOST_AUTO_TEST_CASE(sudoku_s5)
// {
//     const string actual = grab("assets/puzzles/s5.jpg");
//     cout << actual << endl;
//     const string expected = "020093000079000841050001000003050408005806300108030500000100080514000920000340050";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };

// BOOST_AUTO_TEST_CASE(sudoku_s6)
// {
//     const string actual = grab("assets/puzzles/s6.jpg");
//     cout << actual << endl;
//     const string expected = "987004000040658970100020800002001007030945080400200500009080004015497020000500196";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };

// BOOST_AUTO_TEST_CASE(sudoku_s7)
// {
//     const string actual = grab("assets/puzzles/s7.jpg");
//     cout << actual << endl;
//     const string expected = "109537000300006400806400000060703080413060975080109020000005706001900002000628501";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };

// BOOST_AUTO_TEST_CASE(sudoku_s8)
// {
//     const string actual = grab("assets/puzzles/s8.jpg");
//     cout << actual << endl;
//     const string expected = "009100056000060003005300904030750000520806097000012080601003500800070000740008600";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };

// BOOST_AUTO_TEST_CASE(sudoku_s9)
// {
//     const string actual = grab("assets/puzzles/s9.jpg");
//     cout << actual << endl;
//     const string expected = "705060903904058601000907000000012300053000490002430000000105000506780804308020109";
//     cout << expected << endl;
//     bool b = compare(actual, expected);
//     if(b){
//       cout << "---------- OK ----------" << endl;
//     }
//     BOOST_CHECK( b == true);
// };
