# replace XX with the version you have
set(Boost_ADDITIONAL_VERSIONS "1.58" "1.58.0")

# finds boost, triggers an error otherwise
find_package(Boost 1.58 REQUIRED COMPONENTS unit_test_framework)
find_package( OpenCV 3.1.0 REQUIRED core text)


# creates the executable
add_executable(test_executable test.cpp)
# indicates the include paths
target_include_directories(test_executable PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(test_executable PRIVATE ${OpenCV_INCLUDE_DIRS})
# indicates the shared library variant
target_compile_definitions(test_executable PRIVATE "BOOST_TEST_DYN_LINK=1")
# indicates the link paths
target_link_libraries(test_executable ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries( test_executable ${OpenCV_LIBS} )
target_link_libraries( test_executable Add)
target_link_libraries( test_executable Debug)
target_link_libraries( test_executable sudoku)


# declares a test with our executable
add_test(NAME test1 COMMAND test_executable)
