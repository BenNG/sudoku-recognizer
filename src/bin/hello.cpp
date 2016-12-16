#include <string>
#include <vector>
using namespace std; // Or using std::string;
#include "../lib/hello.h"
#include "../lib/sudoku.h"

int main(int argc, char *argv[])
{
    sayHello("LouLou");
    string message = hello();
    cout << message << endl;
    return 0;
}