#include "filesystem.hpp" //namespace FS

std::string fs::directoryCurrent = "/home/robber/samples/";
std::vector<fs::dirEnty> fs::directoryContents;

int fs::dirIndex=0;

int fs::endsWith(const char* haystack, const char* needle)
{
    size_t hlen;
    size_t nlen;
    /* find the length of both arguments - 
    if needle is longer than haystack, haystack can't end with needle */
    hlen = strlen(haystack); 
    nlen = strlen(needle);
    if(nlen > hlen) return 0;

    /* see if the end of haystack equals needle */
    return (strcasecmp(&haystack[hlen-nlen], needle)) == 0;
}

int fs::dirSort(const struct dirent** da, const struct dirent** db) {
  if ((*da)->d_type == (*db)->d_type) {
    return alphasort(db,da);
  }
  if ((*da)->d_type > (*db)->d_type) {
    return -1;
  }
    if ((*da)->d_type < (*db)->d_type) {
    return 1;
  }
}
int fs::dirFilter(const struct dirent* dp) {
  if (std::strcmp(dp->d_name,".") == 0) {
    return 0;
  } else if (dp->d_type == 8) {
    return endsWith(dp->d_name,".wav");
  } else {
    return 1;
  }
}

int fs::getDirectory() {
  struct dirent **filelist;
  int n;
  n = scandir(fs::directoryCurrent.c_str(), &filelist, dirFilter, dirSort);
  if (n<0) {
    return 1;
  } else {
    fs::directoryContents.clear();
    while (n--) {
        fs::directoryContents.push_back({filelist[n]->d_type==4,filelist[n]->d_name});
        free(filelist[n]);
    }
    free(filelist);
    return 0;
  }
}