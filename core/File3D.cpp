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
		mNodeNameLabel = "g ";
	}
	else if(strcmp(fileExtension, "dae") == 0) { // obj extension detected
		mFormat = F3D_DaeFile;
		mNodeNameLabel = "geometry id=";
	}
	else mIsValidFile = false; // unsupported format

	if (mIsValidFile) {
		bool isBinaryRead = true;

		mFileStr = readFile(source, true);
		
		// size_t nodeSearchOffset = 0;
		// size_t foundOffset = mFileStr.find(mNodeNameLabel, nodeSearchOffset); // nodeSearchOffset causes issues on its own
		size_t foundOffset = 0;
		std::string currentName;
		while (mFileStr.find(mNodeNameLabel, foundOffset) != std::string::npos) {
			foundOffset = mNodeNameLabel.length() + mFileStr.find(mNodeNameLabel, foundOffset); // update foundOffset to next position
			currentName = parseNodeName(mFileStr, foundOffset /* + mNodeNameLabel.length() */);
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

	File3D_Node* currentNode = *(mNodeData + nodeNum); // provides easy access to change the target node
}

Geo_Model3D File3D_Node::genRenderObj(){
    Geo_Model3D renderObj;
    // TODO: Populate render object with data

    return renderObj;
}