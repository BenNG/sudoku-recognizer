/**
 * 2/3 mix images in order to take 80% for training and 20% for test
 * All the file will be in the `data/randomized`
 *
 * take all cell pictures in data/X and rename all the images with a part of uuid and the value at the end
 * eg: 6499470e_X
 * then we will use 80% all these images to train and 20% to test the mlp
 *
 * then invoke `featurizeDataForTraining` script
 * */
#define BOOST_FILESYSTEM_VERSION 3

//  As an example program, we don't want to use any deprecated features
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

using namespace boost;
using namespace std;

void create_structure_for_randomization();
std::string sub_uuid(boost::filesystem::path p);
int randomizeData();