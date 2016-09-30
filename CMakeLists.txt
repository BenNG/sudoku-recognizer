cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

find_package( OpenCV 3.1.0 REQUIRED core text)
find_package( Tesseract REQUIRED ) # 3.05 is currently the latest version of the git repository.

include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(${Tesseract_INCLUDE_DIRS})

MESSAGE("OpenCV version : ${OpenCV_VERSION}")
MESSAGE("Tesseract version : ${Tesseract_VERSION}")

add_library(Debug lib/debug.cpp lib/debug.h)
add_library(ExtractPuzzle lib/extractPuzzle.cpp lib/extractPuzzle.h)
add_library(CellHandling lib/cellHandling.cpp lib/cellHandling.h)

#Can manually add the sources using the set command as follows:
set(SOURCES main.cpp )

add_executable( ${PROJECT_NAME} ${SOURCES}  )
add_executable( xor xor.cpp  )
add_executable( letter_recog letter_recog.cpp)
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
target_link_libraries( xor ${OpenCV_LIBS} )
target_link_libraries( letter_recog ${OpenCV_LIBS} )
target_link_libraries( ${PROJECT_NAME} ${Tesseract_LIBRARIES})
target_link_libraries( ${PROJECT_NAME} Debug )
target_link_libraries( ${PROJECT_NAME} ExtractPuzzle )
target_link_libraries( ${PROJECT_NAME} CellHandling )
