#include <ios>
#include <sstream>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdio>
#include <cctype>
#include <vector>
#include <cfloat> // For error handling
#include <cerrno> // For error handling

enum F3D_Format {
	F3D_ObjFile,
	F3D_FbxFile
	// possibly add another "heavier" format such as glTF
};

class File3D_Node {
public:
	File3D_Node(std::string name/*, unsigned propsCount, std::string propsList */) 
		: mName(name)/* , mNumProps(propsCount), mPropsList(propsList) */{}

	std::string getName() { return mName; }
private:
	const std::string mName; // Name of Node
	// const unsigned mNumProps; // Number of properties
	// const std::string mPropsList; // Property list
};

class File3D_DocumentTree {
public:
	File3D_DocumentTree(const char* source); // mNodeData allocation performed here
	~File3D_DocumentTree() { if (mNodeData != nullptr) free(mNodeData); }
private:
	void readNode(unsigned nodeNum); // reads File3D file in ascii encoding mode

	F3D_Format mFormat;
	std::string mFileStr;
	std::string mGeoStartLabel; // label to search for geometry inside of 3D file
	unsigned mNodeCount = 0;
	File3D_Node* mNodeData = nullptr;
	bool mIsValidFile; // used for error checking!
	// std::vector<File3D_Node> mNodes;
	// bool mIsDoneParsing = false; // Set to true once end is reached
};

// ---------------------- Anonymous Functions ---------------------- //

std::string readFile(const char* source, bool isBinaryFile);
std::string getParentDir(const char* arg);
bool checkFormatObj(const char* source);
bool checkFormatFbx(const char* source);
float getFloatFromStr(const std::string& source, size_t startOffset);

// Create a log file for debugging purposes
void logToFile(const char* fileName, std::string logMessage);