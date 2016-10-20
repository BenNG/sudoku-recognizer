#include "hello.h"

void sayHello(string name)
{
    stringstream ss;
    ss << "Hello ";
    ss << name;
    ss << "!";
    cout << ss.str() << endl;
}
