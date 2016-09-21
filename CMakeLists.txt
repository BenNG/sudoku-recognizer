cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS})

add_library(Debug debug.cpp debug.h)

#Can manually add the sources using the set command as follows:
set(SOURCES main.cpp )

add_executable( ${PROJECT_NAME} ${SOURCES}  )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
target_link_libraries( ${PROJECT_NAME} Debug )
