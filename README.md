# Sudoku-recognizer

![ddd.gif](https://bitbucket.org/repo/njp6xM/images/4266970602-ddd.gif)

## Requirement
* [CMake](https://cmake.org/)  
* [OpenCV 3.1.0](http://opencv.org/downloads.html)
    * This [link](http://docs.opencv.org/3.1.0/df/d65/tutorial_table_of_content_introduction.html) might be useful  

## Tests
Once you have installed all the requirements, execute the tests to see if everything is OK
```
# In the build folder
cmake .. && ctest -V
```


## Parse one of the provided puzzle located in `assets/puzzles`
At the root of the project:
```
mkdir build && cd build
cmake ..
make
src/sudokuRecognizer assets/puzzles/s1.jpg # this is path from the root of the project without dots or/and splashes
```

## Tools that can help you during the development process
### Show all extracted puzzles located in `assets/puzzles`
```
# In the build folder
cmake .. && make && src/showExtracted
```

![eee.gif](https://bitbucket.org/repo/njp6xM/images/4118982265-eee.gif)

### Show all extracted cells for `assets/puzzles/s33.jpg`
```
# In the build folder
cmake .. && make && src/showExtracted --puzzleNumber 33 --showCell # will show all cells of `assets/puzzles/s33.jpg`
```

![fff.gif](https://bitbucket.org/repo/njp6xM/images/1253348601-fff.gif)

### Other actions `assets/puzzles/s33.jpg`
```
# In the build folder

cmake .. && make && src/showExtracted # will show all extracted puzzles
cmake .. && make && src/showExtracted --showCell  # will show all extracted cells
cmake .. && make && src/showExtracted --puzzleNumber 33 # will show the extracted puzzle of `assets/puzzles/s33.jpg`
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