cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS})

#Can manually add the sources using the set command as follows:
set(SOURCES main.cpp )

add_executable( DisplayImage ${SOURCES}  )
target_link_libraries( DisplayImage ${OpenCV_LIBS} )
