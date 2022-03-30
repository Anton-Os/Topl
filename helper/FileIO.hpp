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

#define MESSAGE_LOG_FILE "MessageLog.txt" // file name for messages and errors during execution

std::string readFile(const char* source, bool isBinaryFile);
std::string getParentDir(const char* arg);
bool checkFormatObj(const char* source);
void cleanupNumStr(std::string* str); // cleanup string so only numbers and whitespace are left
unsigned getValsCountFromStr(const std::string& source); // extracts number of consecutive values in a string without divisor
unsigned getValsCountFromStr(const std::string& source, unsigned short div); // extracts number of consecutive values in a string divided by div
float getFloatFromStr(const std::string& source, size_t startOffset); // extracts float from str relative to start offset
int getIntFromStr(const std::string& source, size_t startOffset); // extracts int from str relative to start offset
// bool checkFileExtension(const char* source, const char* ext);

// Create a log file for debugging purposes
void logToFile(const char* fileName, std::string logMessage);
// TODO: Include error logging code here