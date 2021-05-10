#include <vector>

#include "FileIO.hpp"

#include "primitives/Geo_Model3D.hpp"

#define MATCHING_LABEL_COUNT 2 // 2 labels are needed for proper parsing
#define FIRST_LABEL 0
#define SECOND_LABEL 1

typedef std::vector<std::string> fileDataExtracts_t; // type for 3D file extracted string data

enum F3D_Format {
	F3D_ObjFile, // OBJ file format support
	F3D_DaeFile // Collada file format support
};

class File3D_Node {
public:
    File3D_Node() : mName("node"){}
	File3D_Node(std::string name) : mName(name){}

	std::string getName() { return mName; }
    Geo_Model3D genRenderObj();
	void appendPosExtractData(std::string data) { mPosData.push_back(data); }
    void appendNormalsExtractData(std::string data){ mNormalsData.push_back(data); }
    void appendTexCoordExtractData(std::string data){ mTexCoordData.push_back(data); }
    void appendIndexExtractData(std::string data){ mIndexData.push_back(data); }
private:
	const std::string mName; // name of Node
    fileDataExtracts_t mPosData;
    fileDataExtracts_t mNormalsData;
    fileDataExtracts_t mTexCoordData;
    fileDataExtracts_t mIndexData;
};

class File3D_DocumentTree {
public:
	File3D_DocumentTree(const char* source); // mNodeData allocation performed here
	~File3D_DocumentTree() {
        for(unsigned n = 0; n < mNodeCount; n++) if(mNodeData + n != nullptr) delete *(mNodeData + n);
        if (mNodeData != nullptr) free(mNodeData); 
     }
private:
	void readNode(unsigned nodeNum); // reads File3D file in ascii encoding mode

    std::string mPosStartLabels[MATCHING_LABEL_COUNT]; // labels to locate parsable vertex positions
    std::string mPosEndLabel; // label to signify end of pos parsing
    std::string mIndexStartLabels[MATCHING_LABEL_COUNT]; // labels to locate parsable indices
    std::string mIndexEndLabel;

	F3D_Format mFormat;
	std::string mFileStr;
	unsigned mNodeCount = 0;
	File3D_Node** mNodeData = nullptr;
	bool mIsValidFile; // used for error checking!
};