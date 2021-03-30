
#include <sstream>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdio>
#include <vector>

class FBX_Node {
public:
	FBX_Node(std::string name) : mName(name) {}

	std::string getName() { return mName; }
private:
	const std::string mName;
};

class FBX_DocumentTree {
public:
	FBX_DocumentTree(const char* source);
private:
	void readNode(unsigned startOffset);

	const char* mFileContent;
	std::vector<FBX_Node> mNodes;
};

std::string readFile(const char* source);
std::string getParentDir(const char* arg);
bool checkFormat_FBX(const char* source);

// Create a log file for debugging purposes
void logToFile(const char* fileName, std::string logMessage);