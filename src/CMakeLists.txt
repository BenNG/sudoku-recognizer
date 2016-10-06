find_package( OpenCV 3.1.0 REQUIRED core text)
find_package( Tesseract REQUIRED ) # 3.05 is currently the latest version of the git repository.
find_package(Boost 1.58 COMPONENTS system filesystem)


include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(${Tesseract_INCLUDE_DIRS})
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()


MESSAGE("OpenCV version : ${OpenCV_VERSION}")
MESSAGE("Tesseract version : ${Tesseract_VERSION}")

add_library(Debug lib/debug.cpp lib/debug.h)
add_library(Add lib/add.cpp lib/add.h)
add_library(ExtractPuzzle lib/extractPuzzle.cpp lib/extractPuzzle.h)
add_library(CellHandling lib/cellHandling.cpp lib/cellHandling.h)
add_library(mlp lib/mlp.cpp lib/mlp.h)

#Can manually add the sources using the set command as follows:
set(SOURCES main.cpp )

add_executable( ${PROJECT_NAME} ${SOURCES}  )
add_executable( createDataForTraining lib/createDataForTraining.cpp)
add_executable( randomizeDataForTraining lib/randomizeDataForTraining.cpp)
add_executable( featurizeDataForTraining lib/featurizeDataForTraining.cpp)


target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
target_link_libraries( ${PROJECT_NAME} ${Tesseract_LIBRARIES})
target_link_libraries( ${PROJECT_NAME} Debug )
target_link_libraries( ${PROJECT_NAME} ExtractPuzzle )
target_link_libraries( ${PROJECT_NAME} CellHandling )
target_link_libraries( ${PROJECT_NAME} mlp )

target_link_libraries( createDataForTraining ${OpenCV_LIBS} )
target_link_libraries( createDataForTraining ${Tesseract_LIBRARIES})
target_link_libraries( createDataForTraining Debug)
target_link_libraries( createDataForTraining ExtractPuzzle)
target_link_libraries( createDataForTraining CellHandling)

target_link_libraries( featurizeDataForTraining ${OpenCV_LIBS} )
target_link_libraries( featurizeDataForTraining CellHandling )

target_link_libraries( mlp ${OpenCV_LIBS} )

if(Boost_FOUND)
    target_link_libraries(createDataForTraining ${Boost_LIBRARIES})
    target_link_libraries(randomizeDataForTraining ${Boost_LIBRARIES})
    target_link_libraries(featurizeDataForTraining ${Boost_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()