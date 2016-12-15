#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <string>
#include "../src/lib/sudoku.h"

using namespace std;
using namespace cv;

Ptr<ml::KNearest> knn = getKnn();

bool compare(string actual, string expected)
{
    stringstream a;
    bool res = true;
    int length = actual.length();
    for (int i = 0; i < length; i++)
    {
        //        cout << actual[i] << endl;
        //        cout << expected[i] << endl;
        if (actual[i] != expected[i])
        {
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
    cout << "universeInOrder" << endl;
    BOOST_CHECK("Hello World !" == "Hello World !");
};
BOOST_AUTO_TEST_CASE(universeInOrder2)
{
    cout << "universeInOrder2" << endl;
    BOOST_CHECK(minuss(2, 2) == 0);
};
BOOST_AUTO_TEST_CASE(sudoku_s0)
{
    cout << "\nsudoku_s0" << endl;

    const string actual = grab("assets/puzzles/s0.jpg", knn);
    cout << actual << endl;
    const string expected = "000604700706000009000005080070020093800000005430010070050200000300000208002301000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s1)
{
    cout << "\nsudoku_s1" << endl;

    const string actual = grab("assets/puzzles/s1.jpg", knn);
    cout << actual << endl;
    const string expected = "007620058014090006006003002008300705070000040309004600700400100100080260890067500";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s2)
{
    cout << "\nsudoku_s2" << endl;

    const string actual = grab("assets/puzzles/s2.jpg", knn);
    cout << actual << endl;
    const string expected = "007620058014090006006003002008300705070000040309004600700400100100080260890067500";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s3)
{
    cout << "\nsudoku_s3" << endl;

    const string actual = grab("assets/puzzles/s3.jpg", knn);
    cout << actual << endl;
    const string expected = "600004100000900508580000740090710800000040000005098030078000021402007000006800005";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s4)
{
    cout << "\nsudoku_s4" << endl;

    const string actual = grab("assets/puzzles/s4.jpg", knn);
    cout << actual << endl;
    const string expected = "600004100000900508580000740090710800000040000005098030078000021402007000006800005";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s5)
{
    cout << "\nsudoku_s5" << endl;

    const string actual = grab("assets/puzzles/s5.jpg", knn);
    cout << actual << endl;
    const string expected = "600004100000900508580000740090710800000040000005098030078000021402007000006800005";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s6)
{
    cout << "\nsudoku_s6" << endl;

    const string actual = grab("assets/puzzles/s6.jpg", knn);
    cout << actual << endl;
    const string expected = "006400750005082060007306089050130900093000840002048070580209600070860200029003400";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s7)
{
    cout << "\nsudoku_s7" << endl;

    const string actual = grab("assets/puzzles/s7.jpg", knn);
    cout << actual << endl;
    const string expected = "006400750005082060007306089050130900093000840002048070580209600070860200029003400";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s8)
{
    cout << "\nsudoku_s8" << endl;

    const string actual = grab("assets/puzzles/s8.jpg", knn);
    cout << actual << endl;
    const string expected = "006400750005082060007306089050130900093000840002048070580209600070860200029003400";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s9)
{
    cout << "\nsudoku_s9" << endl;

    const string actual = grab("assets/puzzles/s9.jpg", knn);
    cout << actual << endl;
    const string expected = "080010040002083001060200003000094350809000704015370000300002070200730600090060030";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s10)
{
    cout << "\nsudoku_s10" << endl;

    const string actual = grab("assets/puzzles/s10.jpg", knn);
    cout << actual << endl;
    const string expected = "080010040002083001060200003000094350809000704015370000300002070200730600090060030";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s11)
{
    cout << "\nsudoku_s11" << endl;

    const string actual = grab("assets/puzzles/s11.jpg", knn);
    cout << actual << endl;
    const string expected = "080010040002083001060200003000094350809000704015370000300002070200730600090060030";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s12)
{
    cout << "\nsudoku_s12" << endl;

    const string actual = grab("assets/puzzles/s12.jpg", knn);
    cout << actual << endl;
    const string expected = "000652090640070501000030780908040000104207908000090605039020000806010059010984000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s13)
{
    cout << "\nsudoku_s13" << endl;

    const string actual = grab("assets/puzzles/s13.jpg", knn);
    cout << actual << endl;
    const string expected = "000652090640070501000030780908040000104207908000090605039020000806010059010984000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s14)
{
    cout << "\nsudoku_s14" << endl;

    const string actual = grab("assets/puzzles/s14.jpg", knn);
    cout << actual << endl;
    const string expected = "000652090640070501000030780908040000104207908000090605039020000806010059010984000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s15)
{
    cout << "\nsudoku_s15" << endl;

    const string actual = grab("assets/puzzles/s15.jpg", knn);
    cout << actual << endl;
    const string expected = "009100056000060003005300904030750000520806097000012080601003500800070000740008600";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s16)
{
    cout << "\nsudoku_s16" << endl;

    const string actual = grab("assets/puzzles/s16.jpg", knn);
    cout << actual << endl;
    const string expected = "009100056000060003005300904030750000520806097000012080601003500800070000740008600";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s17)
{
    cout << "\nsudoku_s17" << endl;

    const string actual = grab("assets/puzzles/s17.jpg", knn);
    cout << actual << endl;
    const string expected = "009100056000060003005300904030750000520806097000012080601003500800070000740008600";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s18)
{
    cout << "\nsudoku_s18" << endl;

    const string actual = grab("assets/puzzles/s18.jpg", knn);
    cout << actual << endl;
    const string expected = "100974080970006000300005027007602000000050000000803100690500008000200095020498006";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s19)
{
    cout << "\nsudoku_s19" << endl;

    const string actual = grab("assets/puzzles/s19.jpg", knn);
    cout << actual << endl;
    const string expected = "100974080970006000300005027007602000000050000000803100690500008000200095020498006";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s20)
{
    cout << "\nsudoku_s20" << endl;

    const string actual = grab("assets/puzzles/s20.jpg", knn);
    cout << actual << endl;
    const string expected = "100974080970006000300005027007602000000050000000803100690500008000200095020498006";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s21)
{
    cout << "\nsudoku_s21" << endl;

    const string actual = grab("assets/puzzles/s21.jpg", knn);
    cout << actual << endl;
    const string expected = "020093000079000841050001000003050408005806300108030500000100080514000920000340050";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s22)
{
    cout << "\nsudoku_s22" << endl;

    const string actual = grab("assets/puzzles/s22.jpg", knn);
    cout << actual << endl;
    const string expected = "020093000079000841050001000003050408005806300108030500000100080514000920000340050";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s23)
{
    cout << "\nsudoku_s23" << endl;

    const string actual = grab("assets/puzzles/s23.jpg", knn);
    cout << actual << endl;
    const string expected = "020093000079000841050001000003050408005806300108030500000100080514000920000340050";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s24)
{
    cout << "\nsudoku_s24" << endl;

    const string actual = grab("assets/puzzles/s24.jpg", knn);
    cout << actual << endl;
    const string expected = "000000805960005700030709400412603000050000040000401372001907060006800057208000000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s25)
{
    cout << "\nsudoku_s25" << endl;

    const string actual = grab("assets/puzzles/s25.jpg", knn);
    cout << actual << endl;
    const string expected = "000000805960005700030709400412603000050000040000401372001907060006800057208000000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s26)
{
    cout << "\nsudoku_s26" << endl;

    const string actual = grab("assets/puzzles/s26.jpg", knn);
    cout << actual << endl;
    const string expected = "000000805960005700030709400412603000050000040000401372001907060006800057208000000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s27)
{
    cout << "\nsudoku_s27" << endl;

    const string actual = grab("assets/puzzles/s27.jpg", knn);
    cout << actual << endl;
    const string expected = "900680710607049020000003900000304568000000000315806000009400000030790201078065004";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s28)
{
    cout << "\nsudoku_s28" << endl;

    const string actual = grab("assets/puzzles/s28.jpg", knn);
    cout << actual << endl;
    const string expected = "900680710607049020000003900000304568000000000315806000009400000030790201078065004";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s29)
{
    cout << "\nsudoku_s29" << endl;

    const string actual = grab("assets/puzzles/s29.jpg", knn);
    cout << actual << endl;
    const string expected = "900680710607049020000003900000304568000000000315806000009400000030790201078065004";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s30)
{
    cout << "\nsudoku_s30" << endl;

    const string actual = grab("assets/puzzles/s30.jpg", knn);
    cout << actual << endl;
    const string expected = "000080057700915003030007910000072600310000025006450000095100070600539004180060000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s31)
{
    cout << "\nsudoku_s31" << endl;

    const string actual = grab("assets/puzzles/s31.jpg", knn);
    cout << actual << endl;
    const string expected = "000080057700915003030007910000072600310000025006450000095100070600539004180060000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s32)
{
    cout << "\nsudoku_s32" << endl;

    const string actual = grab("assets/puzzles/s32.jpg", knn);
    cout << actual << endl;
    const string expected = "000080057700915003030007910000072600310000025006450000095100070600539004180060000";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s33)
{
    cout << "\nsudoku_s33" << endl;

    const string actual = grab("assets/puzzles/s33.jpg", knn);
    cout << actual << endl;
    const string expected = "008007290430100600600083005324000000001406300000000478800920006006005019073800500";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s34)
{
    cout << "\nsudoku_s34" << endl;

    const string actual = grab("assets/puzzles/s34.jpg", knn);
    cout << actual << endl;
    const string expected = "008007290430100600600083005324000000001406300000000478800920006006005019073800500";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s35)
{
    cout << "\nsudoku_s35" << endl;

    const string actual = grab("assets/puzzles/s35.jpg", knn);
    cout << actual << endl;
    const string expected = "008007290430100600600083005324000000001406300000000478800920006006005019073800500";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s36)
{
    cout << "\nsudoku_s36" << endl;

    const string actual = grab("assets/puzzles/s36.jpg", knn);
    cout << actual << endl;
    const string expected = "020093000079000841050001000003050408005806300108030500000100080514000920000340050";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s37)
{
    cout << "\nsudoku_s37" << endl;

    const string actual = grab("assets/puzzles/s37.jpg", knn);
    cout << actual << endl;
    const string expected = "020093000079000841050001000003050408005806300108030500000100080514000920000340050";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};

BOOST_AUTO_TEST_CASE(sudoku_s38)
{
    cout << "\nsudoku_s38" << endl;

    const string actual = grab("assets/puzzles/s38.jpg", knn);
    cout << actual << endl;
    const string expected = "020093000079000841050001000003050408005806300108030500000100080514000920000340050";
    cout << expected << endl;
    bool b = compare(actual, expected);
    if (b)
    {
        cout << "---------- OK ----------" << endl;
    }
    BOOST_CHECK(b == true);
};
