#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>
#include "../src/lib/add.h"
#include "../src/lib/debug.h"

using namespace cv;


BOOST_AUTO_TEST_CASE(universeInOrder)
{
    BOOST_CHECK(add(2, 2) == 4);
};
BOOST_AUTO_TEST_CASE(universeInOrder2)
{
    BOOST_CHECK(minuss(2, 2) == 0);
};