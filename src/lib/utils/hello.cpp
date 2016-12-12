#include <string>
#include <vector>
using namespace std; // Or using std::string;
#include "../hello.h"

int main(int argc, char *argv[])
{
    sayHello("LouLou");

    bool showCell = false, showPuzzle = false, debug = false;
    cout << "argc: " << argc << endl;

    // for (int i = 1; i < argc; i++)
    // {
    //     string arg = argv[i];
    //     if (arg == "--showCell")
    //     {
    //         showCell = true;
    //     }
    //     if (arg == "--showPuzzle")
    //     {
    //         showPuzzle = true;
    //     }
    //     if (arg == "--debug")
    //     {
    //         debug = true;
    //     }
    //     if (arg == "--puzzleNumber")
    //     {
    //         ss << "s" << argv[i + 1] << ".jpg";
    //     }
    // }

    return 0;
}