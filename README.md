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
