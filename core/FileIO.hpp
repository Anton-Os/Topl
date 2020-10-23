#include <sstream>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdio>

// Important file operations
std::string readFile(const char* source);
std::string getParentDir(const char* arg);

// Create a log file for debugging purposes
void logToFile(const char* fileName, std::string logMessage);