#include "FileIO.hpp"

std::string readFile(const char* source){
    std::ifstream file(source);
    if(! file) return nullptr;

    // file.ignore(std::numeric_limits<std::streamsize>::max());
    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::stringstream strStream;
    strStream << file.rdbuf();
    file.close();

    return strStream.str();
}

std::string getParentDir(const char* str){
    const char* strEnd = str + strlen(str); // Traverse to the end of the string
    while(strEnd != str && nullptr == strchr("\\/", *(--strEnd)));

    std::size_t length = strEnd - str; // Gets length of string
    std::string parentDir(str, length); // Gets a subset of the string

    return parentDir;
}

void logToFile(const char* fileName, std::string logMessage){
    std::ofstream file(fileName, std::ofstream::out | std::ofstream::app);

    file << logMessage << "\n\n";

    file.close();
}

bool checkFormat_FBX(const char* source) {
	std::string fileContent = readFile(source);
	std::string magicKey("Kaydara FBX Binary  "); // 21 characters long
	return (magicKey == fileContent.substr(0, 21)) ? true : false;
}