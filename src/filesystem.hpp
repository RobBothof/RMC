#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <dirent.h>
#include <cstring>
#include <vector>
#include <iostream>

namespace fs {
    struct dirEnty {
        bool isDir;
        std::string filename; 
    };

    extern std::string directoryCurrent;
    extern std::vector<dirEnty> directoryContents;
    extern int dirIndex;
    
    int endsWith(const char* haystack, const char* needle);
    int dirSort(const struct dirent** da, const struct dirent** db);
    int dirFilter(const struct dirent* dp);
    int getDirectory();
}
#endif