# sudoku-recognizer

## Requirement

To run this project you need
* [CMake](https://cmake.org/)  
* [OpenCV 3.1.0](http://opencv.org/downloads.html). This [link](http://docs.opencv.org/3.1.0/df/d65/tutorial_table_of_content_introduction.html) might be useful

## Get started
At the root of the project:
```
mkdir build && cd build && cmake .. && make && ./DisplayImage
```

## Actions
* You can see each extrated puzzle by using:
```
cmake .. && make && src/showExtracted
```
* You can see each cell of a puzzle using:
```
cmake .. && make && src/showCells
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