# sudoku-recognizer

## Requirement
* [CMake](https://cmake.org/)  
* [OpenCV 3.1.0](http://opencv.org/downloads.html)
    * This [link](http://docs.opencv.org/3.1.0/df/d65/tutorial_table_of_content_introduction.html) might be useful  
* [boost v1.58](http://www.boost.org/users/history/version_1_58_0.html)  
    * export BOOST_ROOT=/home/benng/Downloads/boost_1_58_0

## Get started
At the root of the project:
```
mkdir build && cd build
cmake ..
make
src/sudokuRecognizer assets/puzzles/s1.jpg # this is path from the root of the project without dots or/and splashes
```

## Run the test to see if everything is OK
```
# In the build folder
cmake .. && ctest -V
```

## Some actions
* You can see some extrated information
```
# In the build folder

cmake .. && make && src/showExtracted # will show all extracted puzzles
cmake .. && make && src/showExtracted --showCell  # will show all extracted cells
cmake .. && make && src/showExtracted --puzzleNumber 33 # will show the extracted puzzle of `assets/puzzles/s33.jpg`
cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell # will show all cells of `assets/puzzles/s33.jpg`
cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell --cellNumber 10 # will show the cell index = 10 of `assets/puzzles/s33.jpg`
cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell --cellNumber 10 --debug # will show the cell index = 10 of `assets/puzzles/s33.jpg` plus the process of extraction
```

## Create the `assets/raw-features.yml` yourself
```
# In the build folder
cmake .. && make && src/prepareData # create training data from s0 --> s35
```

* The knn network can be tested using this:
```
cmake .. && make && src/testKnn
```
> You can iterate over each picture by setting debug=true in the `testKnn` fn  

## Tips
* add set(OpenCV_LIBS opencv_core opencv_objdetect) for HOGDescriptor
* mkdir -p data/{1,2,3,4,5,6,7,8,9}
* prepareData creates "assets/raw-features.yml" and getKnn uses it
* boost library
 * check is the /FindBoost.cmake the supported version
* unrecognize cell ? play with:
  * width_threshold
  * height_threshold