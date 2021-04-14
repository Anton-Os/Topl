#include "FileIO.hpp"

FBX_DocumentTree::FBX_DocumentTree(const char* source) {
	mFileStr = readFile(source, true);

	// TODO: Retrieve global definitions

	while (!mIsDoneParsing)
		readNodeAscii(0); // set this properly
}

unsigned FBX_DocumentTree::readNodeAscii(unsigned startOffseet) {
	const char* fileContent = mFileStr.c_str();

	mIsDoneParsing = true; // set this for last node
	return 0;
}

std::string readFile(const char* source, bool isBinaryFile){
    std::ifstream file = (!isBinaryFile)? std::ifstream(source, std::ios::in) : std::ifstream(source, std::ios::in | std::ios::binary);
	if(!file.is_open()) return std::string(); // error occured, empty string returned

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
	std::string fileContent = readFile(source, true);
	std::string magicKey("Kaydara FBX Binary  "); // 21 characters long
	return (magicKey == fileContent.substr(0, 20)) ? true : false;
	// TODO: Possibly look for version as well
}