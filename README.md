#C++

## seems like [cmake](http://docs.opencv.org/trunk/db/df5/tutorial_linux_gcc_cmake.html) is interesting

## compile and link
```
g++ main.cpp -o main
```

## HelloWorld

```c++
#include <iostream>

int main() {
  std::cout << "HelloWorld !" << std::endl;
  return 0;
}
```

To avoid std:: we can use `namespace std`
```
#include <iostream>

using namespace std;
;
int main() {
  cout << "HelloWorld !" << endl;
  return 0;
}
```

-  il est temps d'entrer dans le vif du sujet et de demander à l'ordinateur de nous prêter un de ses tiroirs

## tips
- Here is an introduction to [cmake](http://derekmolloy.ie/hello-world-introductions-to-cmake/)
- There is a BYTE type on windows to use it on linux use:
```
typedef unsigned char             BYTE;
```
- found the 4 vertices/corner/points of the puzzle which is the biggest area on the pictures
 - findContour
 - approxPolyDP
 - approx.at(0)

- to create the transformation matrix with getPerspectiveTransform start with
 ```

	src_p[0] = cv::Point2f(0.0f, 0.0f);
	src_p[1] = cv::Point2f(   w, 0.0f);
	src_p[2] = cv::Point2f(   w,    h);
	src_p[3] = cv::Point2f(0.0f,    h);

	// to points
	dst_p[0] = cv::Point2f(  hw, 0.0f);
	dst_p[1] = cv::Point2f(   w,   hh);
	dst_p[2] = cv::Point2f(  hw,    h);
	dst_p[3] = cv::Point2f(0.0f,   hh);
 ```
 which rotate the coordinates then move the destination coordinates one by one