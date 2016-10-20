#include "hello.h"

string sayHello(string name)
{
    stringstream ss;
    ss << "Hello ";
    ss << name;
    ss << "!";
    return ss.str();
}
