set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")

find_package( OpenCV 3.1.0 REQUIRED core)

include_directories( ${OpenCV_INCLUDE_DIRS})

set(OpenCV_LIBS opencv_core opencv_objdetect)

MESSAGE("OpenCV version : ${OpenCV_VERSION}")

find_package(OpenCL REQUIRED)


add_library(sudoku lib/sudoku.cpp)
add_library(whereami lib/whereami.c lib/whereami.h)
add_library(hello lib/hello.cpp)

add_executable( recognize recognizer.cpp )
add_executable( mouline main.cpp )
add_executable( showExtracted bin/showExtracted.cpp)
add_executable( showNumberOfCells bin/showNumberOfCells.cpp)
add_executable( prepareData bin/prepareData.cpp)
add_executable( grabNumbers bin/grabNumbers.cpp)
add_executable( testKnn bin/testKnn.cpp)
add_executable( helloBin bin/hello.cpp)

target_link_libraries(grabNumbers sudoku)
target_link_libraries(prepareData sudoku)

target_link_libraries( sudoku ${OpenCV_LIBS})

target_link_libraries( showNumberOfCells ${OpenCV_LIBS} )
target_link_libraries( showNumberOfCells sudoku )

target_link_libraries( showExtracted ${OpenCV_LIBS} )
target_link_libraries( showExtracted sudoku )
target_link_libraries( showExtracted hello )


target_link_libraries( testKnn ${OpenCV_LIBS} )
target_link_libraries( testKnn hello )
target_link_libraries( testKnn sudoku )

target_link_libraries( helloBin hello sudoku)

target_link_libraries( mouline ${OpenCV_LIBS} sudoku )
target_link_libraries( recognize ${OpenCV_LIBS} sudoku )
