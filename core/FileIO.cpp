#include "FileIO.hpp"

File3D_DocumentTree::File3D_DocumentTree(const char* source) {
	const char* fileExtension = &source[strlen(source) - 3]; // fetches last 3 characters
	if (strcmp(fileExtension, "obj") == 0) { // obj extension detected
		mFormat = F3D_ObjFile;
		mGeoStartLabel = "g default";
		mIsValidFile = checkFormatObj(source);
	}
	else if (strcmp(fileExtension, "fbx") == 0) { // fbx extension detected
		mFormat = F3D_FbxFile;
		mGeoStartLabel = "GeometryS";
		mIsValidFile = checkFormatFbx(source);
	}
	else mIsValidFile = false; // unsupported format

	if (mIsValidFile) {
		bool isBinaryRead = (mFormat == F3D_FbxFile) ? true : false; // FBX is a binary read, others are not

		mFileStr = readFile(source, true);
		
		size_t nodeSearchOffset = 0;
		while (mFileStr.find(mGeoStartLabel, nodeSearchOffset) != std::string::npos) { // FBX file specific
			nodeSearchOffset += mFileStr.find(mGeoStartLabel, nodeSearchOffset);
			mNodeCount++;
		}

		if (mNodeCount != 0) { // number of nodes has to increase from 0
			// mNodeCount++; // offset the index value by 1
			mNodeData = (File3D_Node*)malloc(sizeof(File3D_Node) * mNodeCount); // allocates enough space

			for (unsigned n = 0; n < mNodeCount; n++) readNode(n); // read thhrough all nodes in the file
		} else fprintf(stderr, "%s has no valid nodes!", source);
	}
	else fprintf(stderr, "%s is an invalid file!", source); // error occured
}

void File3D_DocumentTree::readNode(unsigned nodeNum) {
	size_t nodeSearchOffset = 0;
	for(unsigned n = 0; n < nodeNum; n++)
		nodeSearchOffset += mFileStr.find(mGeoStartLabel, nodeSearchOffset);
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

bool checkFormatObj(const char* source) {
	std::string fileContent = readFile(source, false); // ascii read
	std::string magicKey("OBJFILE.mtl");
	return (fileContent.find(magicKey) != std::string::npos) ? true :  false;
}

bool checkFormatFbx(const char* source) {
	std::string fileContent = readFile(source, false); // ascii read to save space
	std::string magicKey("Kaydara FBX Binary  "); // 21 characters long
	return (magicKey == fileContent.substr(0, 20)) ? true : false;
	// TODO: Possibly look for version as well
}

float getFloatFromStr(const std::string& source, size_t startOffset){
    if(!isdigit(source.at(startOffset)) && source.at(startOffset) != '-'){ // minus symbol is permitted 
        fprintf(stderr, "Invalid offset provided obtaining float value. Location: %d", startOffset);
        return FLT_MAX;
    }

    std::string currentStrVal;
    while(isdigit(source.at(startOffset)) || source.at(startOffset) == '.' || source.at(startOffset) == '-'){ // minus and dot symbol permitted
        currentStrVal += source.at(startOffset);
        startOffset++;
    }

    return (float)atof(currentStrVal.c_str());
}