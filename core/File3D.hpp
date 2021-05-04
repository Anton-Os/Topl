#include <vector>

#include "FileIO.hpp"

#include "primitives/Geo_Model3D.hpp"

typedef std::vector<std::string> strDataExtracts_t; // type for 3D file extracted string data

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
    void appendPosDataStr(std::string data){ /* mPosData.push_back(data); */ }
    void appendNormalsDataStr(std::string data){ /* mNormalsData.push_back(data); */ }
    void appendTexCoordDataStr(std::string data){ /* mTexCoordData.push_back(data); */ }
    void appendIndexDataStr(std::string data){ /* mIndexData.push_back(data); */ }
private:
	const std::string mName; // Name of Node
    strDataExtracts_t mPosData;
    strDataExtracts_t mNormalsData;
    strDataExtracts_t mTexCoordData;
    strDataExtracts_t mIndexData;
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
};