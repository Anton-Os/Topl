#include <ios>
#include <sstream>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdio>
#include <cctype>
// #include <vector>
#include <climits> // For error handling
#include <cfloat> // For error handling
#include <cerrno> // For error handling

// ---------------------- Anonymous Functions ---------------------- //

std::string readFile(const char* source, bool isBinaryFile);
std::string getParentDir(const char* arg);
bool checkFormatObj(const char* source);
bool checkFormatFbx(const char* source);
float getFloatFromStr(const std::string& source, size_t startOffset); // extracts float from str relative to start offset
int getIntFromStr(const std::string& source, size_t startOffset); // extracts int from str relative to start offset

// Create a log file for debugging purposes
void logToFile(const char* fileName, std::string logMessage);