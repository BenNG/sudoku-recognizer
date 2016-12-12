set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV 3.1.0 REQUIRED core text)

include_directories( ${OpenCV_INCLUDE_DIRS})

set(OpenCV_LIBS opencv_core opencv_objdetect)

MESSAGE("OpenCV version : ${OpenCV_VERSION}")

add_library(opencv lib/utils/opencv.cpp lib/utils/opencv.h)
add_library(hello lib/hello.cpp lib/hello.h)

set(SOURCES main.cpp )

add_executable( ${PROJECT_NAME} ${SOURCES} )
add_executable( showExtracted lib/utils/showExtracted.cpp)
add_executable( prepareData lib/utils/prepareData.cpp)
add_executable( testKnn lib/utils/testKnn.cpp)
add_executable( helloBin lib/utils/hello.cpp)

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
