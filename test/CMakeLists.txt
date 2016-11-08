set(Boost_ADDITIONAL_VERSIONS "1.58" "1.58.0")

find_package(Boost 1.58 REQUIRED COMPONENTS unit_test_framework)


add_executable(test_executable test.cpp)
target_include_directories(test_executable PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(test_executable PRIVATE ${OpenCV_INCLUDE_DIRS})
target_compile_definitions(test_executable PRIVATE "BOOST_TEST_DYN_LINK=1")
target_link_libraries(test_executable ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries( test_executable ${OpenCV_LIBS} )
target_link_libraries( test_executable opencv)

add_test(NAME test1 COMMAND test_executable)
