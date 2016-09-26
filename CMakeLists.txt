cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

find_package( OpenCV 3.1.0 REQUIRED core text)
find_package( Tesseract REQUIRED ) # 3.05 is currently the latest version of the git repository.

include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(${Tesseract_INCLUDE_DIRS})

MESSAGE("OpenCV version : ${OpenCV_VERSION}")
MESSAGE("Tesseract version : ${Tesseract_VERSION}")

add_library(Debug debug.cpp debug.h)

#Can manually add the sources using the set command as follows:
set(SOURCES main.cpp )

add_executable( ${PROJECT_NAME} ${SOURCES}  )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
target_link_libraries( ${PROJECT_NAME} ${Tesseract_LIBRARIES})
target_link_libraries( ${PROJECT_NAME} Debug )
