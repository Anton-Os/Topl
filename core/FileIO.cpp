#include "FileIO.hpp"

FBX_DocumentTree::FBX_DocumentTree(const char* source) {
	mFileContent = readFile(source).c_str();

	readNode(27); // 27 is the offset of the first node in fbx!
}

unsigned FBX_DocumentTree::readNode(unsigned startOffseet) {
    // Isolates the name for a given node
    char nameLenChar = *(mFileContent + startOffseet + mNameLen_relOffset); // conversion required from const char* to char
    unsigned nameLen = (unsigned)atoi(&nameLenChar);
    std::string nameStr = std::string(mFileContent + startOffseet + mName_relOffset, (size_t)nameLen);

    // Isolates the number of properties for given node
    char numProps_chars[4] = {
        *(mFileContent + startOffseet + mNumProps_relOffset + 0), *(mFileContent + startOffseet + mNumProps_relOffset + 1), *(mFileContent + startOffseet + mNumProps_relOffset + 2), *(mFileContent + startOffseet + mNumProps_relOffset + 3) 
    };
    unsigned numProps = (unsigned)atoi(&numProps_chars[0]);

     // Isolates the property list for a given node
    char propsListLen_chars[4] = { 
        *(mFileContent + startOffseet + mPropsListLen_relOffset + 0), *(mFileContent + startOffseet + mPropsListLen_relOffset + 1), *(mFileContent + startOffseet + mPropsListLen_relOffset + 2), *(mFileContent + startOffseet + mPropsListLen_relOffset + 3)
    };
    unsigned propsListLen = (unsigned)atoi(&propsListLen_chars[0]);
    std::string propsListStr = std::string(mFileContent + startOffseet + mName_relOffset + nameLen, (size_t)propsListLen);
	
    FBX_Node node = FBX_Node(nameStr, numProps, propsListStr);
	mNodes.push_back(node);

	return 0; // TODO: Return the new offset to begin reading!
}

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
	return (magicKey == fileContent.substr(0, 20)) ? true : false;
	// TODO: Possibly look for version as well
}