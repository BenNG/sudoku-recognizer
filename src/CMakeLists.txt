find_package( OpenCV 3.1.0 REQUIRED core text)
find_package( Tesseract REQUIRED ) # 3.05 is currently the latest version of the git repository.
find_package(Boost 1.58 COMPONENTS system filesystem program_options)

include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(${Tesseract_INCLUDE_DIRS})
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

MESSAGE("OpenCV version : ${OpenCV_VERSION}")
MESSAGE("Tesseract version : ${Tesseract_VERSION}")

add_library(Debug lib/debug.cpp lib/debug.h)
add_library(Add lib/add.cpp lib/add.h)
add_library(picture lib/utils/picture.cpp lib/utils/picture.h)
add_library(CellHandling lib/utils/cell.cpp lib/utils/cell.h)
add_library(mlp lib/mlp.cpp lib/mlp.h)
add_library(createDataForTraining lib/preTraining/createDataForTraining.cpp lib/preTraining/createDataForTraining.h)
add_library(randomizeDataForTraining lib/preTraining/randomizeDataForTraining.cpp lib/preTraining/randomizeDataForTraining.h)
add_library(featurizeDataForTraining lib/preTraining/featurizeDataForTraining.cpp lib/preTraining/featurizeDataForTraining.h)
add_library(sudoku lib/utils/sudoku.cpp lib/utils/sudoku.h)

#Can manually add the sources using the set command as follows:
set(SOURCES main.cpp )

add_executable( ${PROJECT_NAME} ${SOURCES} )
add_executable( showExtracted lib/utils/showExtracted.cpp)
# add_executable( knn lib/knn.cpp)
#add_executable( randomizeDataForTraining lib/randomizeDataForTraining.cpp)
#add_executable( featurizeDataForTraining lib/featurizeDataForTraining.cpp)

target_link_libraries( picture Debug)

target_link_libraries( createDataForTraining ${OpenCV_LIBS} )
target_link_libraries( createDataForTraining ${Tesseract_LIBRARIES})
target_link_libraries( createDataForTraining Debug)
target_link_libraries( createDataForTraining picture)
target_link_libraries( createDataForTraining CellHandling)

target_link_libraries( featurizeDataForTraining ${OpenCV_LIBS} )
target_link_libraries( featurizeDataForTraining CellHandling )

target_link_libraries( showExtracted CellHandling )
target_link_libraries( showExtracted Debug )
target_link_libraries( showExtracted mlp )
target_link_libraries( showExtracted picture )
target_link_libraries( showExtracted ${OpenCV_LIBS} )

target_link_libraries( mlp ${OpenCV_LIBS} )

# target_link_libraries( knn ${OpenCV_LIBS} )
# target_link_libraries( knn Debug )

target_link_libraries( sudoku ${OpenCV_LIBS} )
target_link_libraries( sudoku randomizeDataForTraining )
target_link_libraries( sudoku createDataForTraining )
target_link_libraries( sudoku featurizeDataForTraining )
target_link_libraries( sudoku mlp )

if(Boost_FOUND)
    target_link_libraries(showExtracted ${Boost_LIBRARIES} )
    target_link_libraries(createDataForTraining ${Boost_LIBRARIES})
    target_link_libraries(randomizeDataForTraining ${Boost_LIBRARIES})
    target_link_libraries(featurizeDataForTraining ${Boost_LIBRARIES})
    target_link_libraries(sudoku ${Boost_LIBRARIES} )
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
  #  target_link_libraries(knn ${Boost_LIBRARIES})
endif()

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
target_link_libraries( ${PROJECT_NAME} ${Tesseract_LIBRARIES})
target_link_libraries( ${PROJECT_NAME} Debug )
target_link_libraries( ${PROJECT_NAME} picture )
target_link_libraries( ${PROJECT_NAME} CellHandling )
target_link_libraries( ${PROJECT_NAME} mlp )
target_link_libraries( ${PROJECT_NAME} sudoku )
