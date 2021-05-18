#include "File3D.hpp"

static std::string parseNodeName(const std::string& fileContent, size_t offset){
	if (offset > fileContent.size()) return std::string(); // safety check

	std::string name;
	while(!isalpha(fileContent.at(offset))) offset++; // search for next alpha character
	while (fileContent.at(offset) != '\n' && !isspace(fileContent.at(offset)) && fileContent.at(offset) != '_' && fileContent.at(offset) != '\"') {
		name += fileContent.at(offset); // keep appending valid characters
		offset++;
	}

	if(name == "default") return std::string(); // node cannot be named default because of obj parsing!
	else return name;
}

File3D_DocumentTree::File3D_DocumentTree(const char* source) {
	const char* fileExtension = &source[strlen(source) - 3]; // fetches last 3 characters
	if(strcmp(fileExtension, "obj") == 0) { // obj extension detected
		mFormat = F3D_ObjFile;
		mParseType = F3D_CheckLine;
		// File3D_DocumentTree::File3D_LineParseAttribs parseAttribs = mLineParseAttribs;
		mLineParseAttribs = File3D_LineParseAttribs("v ", "vn ", "vt ", "f ");
		mNodeNameLabel = "g ";
	}
	else if(strcmp(fileExtension, "dae") == 0) { // obj extension detected
		mFormat = F3D_DaeFile;
		mParseType = F3D_CheckTag;
		mTagParseAttribs = File3D_TagParseAttribs(
			"float_array", "POSITION",
			"float_array", "Normal",
			"float_array", "UV",
			"p", ""
		);
		mNodeNameLabel = "geometry id=";
	}
	else mIsValidFile = false; // unsupported format

	if (mIsValidFile) {
		bool isBinaryRead = true;

		mFileSource = readFile(source, true);

		size_t foundOffset = 0;
		std::string currentName;
		while (mFileSource.find(mNodeNameLabel, foundOffset) != std::string::npos) {
			foundOffset = mNodeNameLabel.length() + mFileSource.find(mNodeNameLabel, foundOffset); // update foundOffset to next position
			currentName = parseNodeName(mFileSource, foundOffset /* + mNodeNameLabel.length() */);
			if(!currentName.empty()) mNodeNames.push_back(currentName);
		}

		if (mNodeNames.size() != 0) { // number of nodes has to increase from 0
			mNodeData = (File3D_Node**)malloc(sizeof(File3D_Node) * mNodeNames.size()); // allocates enough space

			for(unsigned n = 0; n < mNodeNames.size(); n++) 
				*(mNodeData + n) = new File3D_Node(currentName); // iterate through creating properly named

			for (unsigned n = 0; n < mNodeNames.size(); n++) 
				readNode(n); // iterate through populating nodes with data

		} else fprintf(stderr, "%s has no valid nodes!", source);
	}
	else fprintf(stderr, "%s is an invalid file!", source); // error occured
}

void File3D_DocumentTree::readNode(unsigned nodeNum) {
	if(nodeNum > mNodeNames.size()){
		fprintf(stderr, "Node %d is out of range! Max nodes detected is at %d", nodeNum, mNodeNames.size());
		return;
	}

	size_t foundOffset = 0;
	File3D_Node* currentNode = *(mNodeData + nodeNum); // provides add data to the target node
	std::string currentLine;

	if (mParseType == F3D_CheckLine) {
		for (unsigned n = 0; n < nodeNum + 1; n++) {
			// Skip unecessary lines, check for matching lines
			if (n == nodeNum) break;
			else {
				// Skip over older node data here
			}
		}
		// while(currentLine.substr(0, ))
	}
	else if (mParseType == F3D_CheckTag) {
		// Perform parsing by matching tag check
	}
}

Geo_Model3D File3D_Node::genRenderObj(){
    Geo_Model3D renderObj;
    // TODO: Populate render object with data

    return renderObj;
}