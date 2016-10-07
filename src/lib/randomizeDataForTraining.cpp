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
#include "randomizeDataForTraining.h"


using namespace boost;
using namespace std;



void create_structure_for_randomization() {

    boost::filesystem::path randomized("data/randomized");

    if (!boost::filesystem::exists(randomized)) {
        boost::filesystem::create_directories(randomized);
    } else {
        boost::filesystem::remove_all(randomized);
        boost::filesystem::create_directories(randomized);
    }
}

std::string sub_uuid(boost::filesystem::path p) {
    vector<string> strs;

    string line(p.string());

    boost::split(strs,line,boost::is_any_of("/"));
//    std::cout << strs[1] << std::endl;
//    string value(strs[1]);

    std::stringstream ss;
    boost::uuids::uuid uuid = boost::uuids::random_generator()();

    const std::string tmp = boost::lexical_cast<std::string>(uuid);
    size_t first = tmp.find_first_of("-");
    ss << tmp.substr(0, first);
    ss << "_";
    ss << strs[1]; // value
    ss << p.filename().extension().string();
    return ss.str();
}

int randomizeData() {
    create_structure_for_randomization();
//    boost::filesystem::path data("data");
//    boost::filesystem::path data1("data/1");
//    boost::filesystem::path data2("data/2");
//    boost::filesystem::path data3("data/3");
//    boost::filesystem::path data4("data/4");
//    boost::filesystem::path data5("data/5");
//    boost::filesystem::path data6("data/6");
//    boost::filesystem::path data7("data/7");
//    boost::filesystem::path data8("data/8");

    const char *files[] = {
            "data/1",
            "data/2",
            "data/3",
            "data/4",
            "data/5",
            "data/6",
            "data/7",
            "data/8",
            "data/9",
    };

    unsigned nb_files = sizeof(files) / sizeof(const char *);

    for (unsigned j = 0; j < nb_files; ++j) {

        boost::filesystem::path p(files[j]);

        unsigned long file_count = 0;
        unsigned long dir_count = 0;
        unsigned long other_count = 0;
        unsigned long err_count = 0;

        if (!boost::filesystem::exists(p)) {
            std::cout << "\nNot found: " << p << std::endl;
            return 1;
        }

        if (boost::filesystem::is_directory(p)) {
            // std::cout << "\nIn directory: " << p << "\n\n";
            boost::filesystem::directory_iterator end_iter;
            for (boost::filesystem::directory_iterator dir_itr(p);
                 dir_itr != end_iter;
                 ++dir_itr) {
                try {
                    if (boost::filesystem::is_directory(dir_itr->status())) {
                        ++dir_count;
                        // std::cout << dir_itr->path().filename() << " [directory]\n";
                    } else if (boost::filesystem::is_regular_file(dir_itr->status())) {
                        ++file_count;
                        boost::filesystem::path source(dir_itr->path());

//                        std::cout << p << "\n";

                        std::string s(sub_uuid(source));
//                        std::cout << s << std::endl;

                        boost::filesystem::path dest("data/randomized/" + s);

                        boost::filesystem::copy(source, dest);


                    } else {
                        ++other_count;
                        // std::cout << dir_itr->path().filename() << " [other]\n";
                    }

                }
                catch (const std::exception &ex) {
                    ++err_count;
                    std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
                }
            }
//        std::cout << "\n" << file_count << " files\n"
//                  << dir_count << " directories\n"
//                  << other_count << " others\n"
//                  << err_count << " errors\n";
        } else // must be a file
        {
//        std::cout << "\nFound: " << p << "\n";
        }

    }
    return 0;
}