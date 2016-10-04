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

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

namespace fs = boost::filesystem;


void create_structure() {

    fs::path randomized("data/randomized");

    if (!fs::exists(randomized)) {
        boost::filesystem::create_directories(randomized);
    } else {
        boost::filesystem::remove_all(randomized);
        boost::filesystem::create_directories(randomized);
    }
}

std::string sub_uuid() {
//    std::stringstream ss;
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
//    std::cout << uuid << std::endl;

    const std::string tmp = boost::lexical_cast<std::string>(uuid);

    size_t first = tmp.find_first_of("-");
    if (first == std::string::npos) return tmp;
    return tmp.substr(0, first);
}

int main(int argc, char *argv[]) {
    create_structure();
//    fs::path data("data");
//    fs::path data1("data/1");
//    fs::path data2("data/2");
//    fs::path data3("data/3");
//    fs::path data4("data/4");
//    fs::path data5("data/5");
//    fs::path data6("data/6");
//    fs::path data7("data/7");
//    fs::path data8("data/8");

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

        fs::path p(files[j]);

        unsigned long file_count = 0;
        unsigned long dir_count = 0;
        unsigned long other_count = 0;
        unsigned long err_count = 0;

        if (!fs::exists(p)) {
            std::cout << "\nNot found: " << p << std::endl;
            return 1;
        }

        if (fs::is_directory(p)) {
            // std::cout << "\nIn directory: " << p << "\n\n";
            fs::directory_iterator end_iter;
            for (fs::directory_iterator dir_itr(p);
                 dir_itr != end_iter;
                 ++dir_itr) {
                try {
                    if (fs::is_directory(dir_itr->status())) {
                        ++dir_count;
                        // std::cout << dir_itr->path().filename() << " [directory]\n";
                    } else if (fs::is_regular_file(dir_itr->status())) {
                        ++file_count;

                        std::cout << dir_itr->path() << "\n";

                        std::string s(sub_uuid());
                        std::cout << s << std::endl;

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