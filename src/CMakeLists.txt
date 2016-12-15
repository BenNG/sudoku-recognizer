set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV 3.1.0 REQUIRED core text)

include_directories( ${OpenCV_INCLUDE_DIRS})

set(OpenCV_LIBS opencv_core opencv_objdetect)

MESSAGE("OpenCV version : ${OpenCV_VERSION}")

add_library(opencv lib/opencv.cpp)
add_library(hello lib/hello.cpp)

set(SOURCES main.cpp )

add_executable( ${PROJECT_NAME} ${SOURCES} )
add_executable( showExtracted bin/showExtracted.cpp)
add_executable( prepareData bin/prepareData.cpp)
add_executable( testKnn bin/testKnn.cpp)
add_executable( helloBin bin/hello.cpp)

target_link_libraries(prepareData opencv)

target_link_libraries( opencv ${OpenCV_LIBS})

target_link_libraries( showExtracted ${OpenCV_LIBS} )
target_link_libraries( showExtracted opencv )
target_link_libraries( showExtracted hello )


target_link_libraries( testKnn ${OpenCV_LIBS} )
target_link_libraries( testKnn hello )
target_link_libraries( testKnn opencv )

target_link_libraries( helloBin hello )

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
target_link_libraries( ${PROJECT_NAME} opencv )
