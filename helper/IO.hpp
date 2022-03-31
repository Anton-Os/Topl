#ifndef IO_H

#include <ios>
#include <sstream>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdio>
#include <cctype>

#define MESSAGE_LOG_FILE "_MessageLog.txt" // file name for messages and errors during execution

std::string readFile(const char* source, bool isBinaryFile);
std::string getParentDir(const char* arg);
bool checkFormatObj(const char* source);
void cleanupNumStr(std::string* str); // cleanup string so only numbers and whitespace are left
unsigned getValsCountFromStr(const std::string& source); // extracts number of consecutive values in a string without divisor
unsigned getValsCountFromStr(const std::string& source, unsigned short div); // extracts number of consecutive values in a string divided by div
float getFloatFromStr(const std::string& source, size_t startOffset); // extracts float from str relative to start offset
int getIntFromStr(const std::string& source, size_t startOffset); // extracts int from str relative to start offset
// bool checkFileExtension(const char* source, const char* ext);

enum MESSAGE_Type {
    MESSAGE_Comment, // for status
    MESSAGE_Question, // for inquiries
    MESSAGE_Exclaim, // for errors
};

// Create a log file for debugging purposes
void logMessage(enum MESSAGE_Type type, std::string message);
void logMessage(std::string message);

#define IO_H
#endif