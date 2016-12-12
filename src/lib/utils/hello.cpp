#include <string>
#include <vector>
using namespace std; // Or using std::string;
#include "../hello.h"
#include <dirent.h>

int getdir(string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == NULL)
    {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

int getNumberOfFilesInFolder(string dir)
{
    vector<string> files;
    getdir(dir, files);
    return files.size() - 2;
}

int main(int argc, char **argv)
{
    sayHello("LouLou");
    string dir = string("../assets/puzzles");
    vector<string> files = vector<string>();

    int num = getNumberOfFilesInFolder(dir);



    cout << num << endl;

    // for (unsigned int i = 0; i < files.size(); i++)
    // {
    //     cout << files[i] << endl;
    // }
    return 0;
}