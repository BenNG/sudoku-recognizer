# sudoku-recognizer

## Requirement

[This](http://docs.opencv.org/3.1.0/df/d65/tutorial_table_of_content_introduction.html) will help you to install OpenCV and OpenCV_contrib

[OpenCV 3.1.0](http://opencv.org/downloads.html) I recommend using the official website instead of the github repo

[opencv_contrib](https://github.com/opencv/opencv_contrib/releases/tag/3.1.0)

[CMake](https://cmake.org/)



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
* use this to rename:
* tesseract only use in dev ? for creating the cell in the data folder
* boost library
 * check is the /FindBoost.cmake the supported version