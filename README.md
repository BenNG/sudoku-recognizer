# sudoku-recognizer

## Requirement

To run this project you need
* [CMake](https://cmake.org/)
* [OpenCV 3.1.0](http://opencv.org/downloads.html) and [OpenCV_contrib](https://github.com/opencv/opencv_contrib). This [link](http://docs.opencv.org/3.1.0/df/d65/tutorial_table_of_content_introduction.html) might be useful
* [Tesseract](https://github.com/tesseract-ocr) (not needed anymore)
  * build the project
    * clone the repo
    * mkdir build
    * cmake ..
  * use it in the project
    * Tesseract_DIR=/keep/Repo/tesseract/build cmake .. # needed for the first time after you can use cmake .. only

## Get started

* Go to the root of the project
* mkdir build
* cd build
* Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/DisplayImage ../assets/puzzles/s0.jpg

## Actions
* You can see each extrated puzzle by using:
```
Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted
```
* You can see each cell of a puzzle using:
```
Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showCells
``` 
* The knn network can be tested using this:
```
Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/testKnn
```
> You can iterate over each picture by setting debug=true in the `testKnn` fn  


## adding tesseract
```
- clone the repo
- mkdir build
cmake ..

- built sudoku project
Tesseract_DIR=/keep/Repo/tesseract/build cmake ..
in CMakeLists.txt add:

find_package( Tesseract REQUIRED ) # 3.05 is currently the latest version of the git repository.
include_directories(${Tesseract_INCLUDE_DIRS})
MESSAGE("Tesseract version : ${Tesseract_VERSION}") # to test
target_link_libraries( ${PROJECT_NAME} ${Tesseract_LIBRARIES})
```

## Build the project

At the root of the project:
```
mkdir build
cd build
cmake ..
make && ./DisplayImage
```

## tips
* mkdir -p data/{1,2,3,4,5,6,7,8,9}
* tesseract only use in dev ? for creating the cell in the data folder
* boost library
 * check is the /FindBoost.cmake the supported version
* Artificial neural network - Multilayer perceptron
  * This Artificial neural network (ann) need to be trained, either featuredDataForTraining.xml or trained_data is needed to work properly. If both the files don't exist, the system will created and use featuredDataForTraining.xml to create trained_data and after the system will use trained_data to predict.
* Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted
* showExtracted:
  * Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted # see all images in assets/puzzles
  * Tesseract_DIR=/keep/Repo/tesseract/build cmake .. && make && src/showExtracted assets/puzzles/s6.jpg # show detailed steps on 1 file
