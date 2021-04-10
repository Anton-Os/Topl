
#include <sstream>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdio>
// #include <cstdint>
#include <vector>

class FBX_Node {
public:
	FBX_Node(std::string name, unsigned propsCount, std::string propsList) 
		: mName(name), mNumProps(propsCount), mPropsList(propsList) {}

	std::string getName() { return mName; }
private:
	const std::string mName; // Name of Node
	const unsigned mNumProps; // Number of properties
	const std::string mPropsList; // Property list
};

class FBX_DocumentTree {
public:
	FBX_DocumentTree(const char* source);
private:
	unsigned readNode(unsigned startOffset); // Returns byte relOffset of the next node

	const char* mFileContent;
	std::vector<FBX_Node> mNodes;

	// Here are relOffset values to help navigate each node
	// For reference https://code.blender.org/2013/08/fbx-binary-file-format-specification/
	const unsigned mEndOffset_relOffset = 0;
	const unsigned mNumProps_relOffset = 4;
	const unsigned mPropsListLen_relOffset = 8;
	const unsigned mNameLen_relOffset = 12;
	const unsigned mName_relOffset = 13;
};

// ---------------------- Anonymous Functions ---------------------- //

std::string readFile(const char* source);
std::string getParentDir(const char* arg);
bool checkFormat_FBX(const char* source);

// Create a log file for debugging purposes
void logToFile(const char* fileName, std::string logMessage);