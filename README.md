# sudoku-recognizer

## Requirement

To run this project you need  
* [CMake](https://cmake.org/)  
* [OpenCV 3.1.0](http://opencv.org/downloads.html). This [link](http://docs.opencv.org/3.1.0/df/d65/tutorial_table_of_content_introduction.html) might be useful  
* [boost v1.58](http://www.boost.org/users/history/version_1_58_0.html)
  * export BOOST_ROOT=/home/benng/Downloads/boost_1_58_0

## Get started
At the root of the project:
```
mkdir build && cd build
cmake ..
make
src/sudokuRecognizer assets/puzzles/s1.jpg # this is path from the root of the project without dots or/and splash
```

## Actions
* You can see some extrated information
```
cmake .. && make && src/showExtracted # will show all extracted puzzles
cmake .. && make && src/showExtracted --showCell  # will show all extracted cells
cmake .. && make && src/showExtracted --puzzleNumber 33 # will show the extracted puzzle of `assets/puzzles/s33.jpg`
cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell # will show all cells of `assets/puzzles/s33.jpg`
cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell --cellNumber 10 # will show the cell index = 10 of `assets/puzzles/s33.jpg`
cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell --cellNumber 10 --debug # will show the cell index = 10 of `assets/puzzles/s33.jpg` plus the process of extraction
``` 
* The knn network can be tested using this:
```
cmake .. && make && src/testKnn
```
> You can iterate over each picture by setting debug=true in the `testKnn` fn  

## tips
* add set(OpenCV_LIBS opencv_core opencv_objdetect) for HOGDescriptor
* mkdir -p data/{1,2,3,4,5,6,7,8,9}
* tesseract only use in dev ? for creating the cell in the data folder
* boost library
 * check is the /FindBoost.cmake the supported version
* Artificial neural network - Multilayer perceptron
  * This Artificial neural network (ann) need to be trained, either featuredDataForTraining.xml or trained_data is needed to work properly. If both the files don't exist, the system will created and use featuredDataForTraining.xml to create trained_data and after the system will use trained_data to predict.
* prepareData creates "assets/raw-features.yml" and getKnn uses it
* unrecognize cell ? play with:
  * width_threshold
  * height_threshold