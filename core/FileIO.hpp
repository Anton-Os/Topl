#include <sstream>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdio>

std::string readFile(const char* source);
std::string getParentDir(const char* arg);

/* #ifdef _WIN32
    void convertTo_wchar(const wchar_t* wc, const char* c);
#endif */