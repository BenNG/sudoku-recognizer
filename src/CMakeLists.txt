FIND_PACKAGE(CURL)
IF(CURL_FOUND)
  INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
  SET(requiredlibs ${requiredlibs} ${CURL_LIBRARIES} )
ELSE(CURL_FOUND)
  MESSAGE(FATAL_ERROR "Could not find the CURL library and development files.")
ENDIF(CURL_FOUND)

find_package( OpenCV 3.1.0 REQUIRED core text)

include_directories( ${OpenCV_INCLUDE_DIRS})

set(OpenCV_LIBS opencv_core opencv_objdetect)

MESSAGE("OpenCV version : ${OpenCV_VERSION}")

add_library(sudoku lib/sudoku.cpp)
add_library(whereami lib/whereami.c lib/whereami.h)
add_library(hello lib/hello.cpp)

add_executable( recognize recognizer.cpp )
add_executable( mouline main.cpp )
add_executable( showExtracted bin/showExtracted.cpp)
add_executable( prepareData bin/prepareData.cpp)
add_executable( testKnn bin/testKnn.cpp)
add_executable( helloBin bin/hello.cpp)

target_link_libraries(prepareData sudoku)

target_link_libraries( sudoku ${OpenCV_LIBS} curl)

target_link_libraries( showExtracted ${OpenCV_LIBS} )
target_link_libraries( showExtracted sudoku )
target_link_libraries( showExtracted hello )


target_link_libraries( testKnn ${OpenCV_LIBS} )
target_link_libraries( testKnn hello )
target_link_libraries( testKnn sudoku )

target_link_libraries( helloBin hello sudoku)

target_link_libraries( mouline ${OpenCV_LIBS} sudoku )
target_link_libraries( recognize ${OpenCV_LIBS} sudoku )
