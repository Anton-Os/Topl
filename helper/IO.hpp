#ifndef IO_H

#include <ios>
#include <sstream>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdio>
#include <cctype>
#include <cfloat>
#include <climits>

// Files

std::string readFile(const char* source);
std::string readFileBinary(const char* source);
std::string getParentDir(const char* arg);
// bool checkFileExtension(const char* source, const char* ext);
float getFloatFromStr(const std::string& source, size_t startOffset); // extracts float from str relative to start offset
int getIntFromStr(const std::string& source, size_t startOffset); // extracts int from str relative to start offset

// Logging

#define MESSAGE_LOG_FILE "_MessageLog.txt" // file name for messages and errors during execution

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