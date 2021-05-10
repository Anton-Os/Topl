#include "File3D.hpp"

File3D_DocumentTree::File3D_DocumentTree(const char* source) {
	const char* fileExtension = &source[strlen(source) - 3]; // fetches last 3 characters
	if(strcmp(fileExtension, "obj") == 0) { // obj extension detected
		mFormat = F3D_ObjFile;
		// mPosStartLabels = { std::string("g"), std::string("default") };
		mPosStartLabels[FIRST_LABEL] = "g";
		mPosStartLabels[SECOND_LABEL] = "default";
		mPosEndLabel = "vt"; // texCoords come next sequentially in obj file spec
		// mIndexStartLabels = { "usemtl", "initialShadingGroup" };
		mIndexStartLabels[FIRST_LABEL] = "usemtl";
		mIndexStartLabels[SECOND_LABEL] = "initialShadingGroup";
		mIndexEndLabel = "g"; // g label follows indices
		mIsValidFile = checkFormatObj(source);
	}
	else if(strcmp(fileExtension, "dae") == 0) { // obj extension detected
		mFormat = F3D_DaeFile;
		// mPosStartLabels = { "<float_array", "POSITION-array" };
		mPosStartLabels[FIRST_LABEL] = "<float_array";
		mPosStartLabels[SECOND_LABEL] = "POSITION-array";
		mPosEndLabel = "</float_array>";
		// mIndexStartLabels = { "", "<p>" };
		mIndexStartLabels[FIRST_LABEL] = "<p"; // breaking a tag into 2
		mIndexStartLabels[SECOND_LABEL] = ">"; // breaking a tag into 2
		mIndexEndLabel = "        </triangles>"; // attempt to remove whitespace
		// mIsValidFile = checkFormatDae(source);
	}
	else mIsValidFile = false; // unsupported format

	if (mIsValidFile) {
		bool isBinaryRead = true;

		mFileStr = readFile(source, true);
		
		size_t nodeSearchOffset = 0;
		while (mFileStr.find(mPosStartLabels[SECOND_LABEL], nodeSearchOffset) != std::string::npos) { // TODO: Check for matching labels
			nodeSearchOffset += mFileStr.find(mPosStartLabels[SECOND_LABEL], nodeSearchOffset);
			mNodeCount++;
		}

		if (mNodeCount != 0) { // number of nodes has to increase from 0
			mNodeData = (File3D_Node**)malloc(sizeof(File3D_Node) * mNodeCount); // allocates enough space
			for(unsigned n = 0; n < mNodeCount; n++) *(mNodeData + n) = new File3D_Node(); // empty node

			for (unsigned n = 0; n < mNodeCount; n++) readNode(n); // read thhrough all nodes in the file
		} else fprintf(stderr, "%s has no valid nodes!", source);
	}
	else fprintf(stderr, "%s is an invalid file!", source); // error occured
}

void File3D_DocumentTree::readNode(unsigned nodeNum) {
	if(nodeNum > mNodeCount){
		fprintf(stderr, "Node %d is out of range! Max nodes detected is at %d", nodeNum, mNodeCount);
		return;
	}

	File3D_Node* currentNode = *(mNodeData + nodeNum); // provides easy access to change the target node
	size_t nodeSearchOffset = 0;
	
	// find the cooresponding label for vertex positions
	for(unsigned n = 0; n <= nodeNum; n++)
		nodeSearchOffset += mFileStr.find(mPosStartLabels[SECOND_LABEL], nodeSearchOffset); // TODO: Check for matching labels
	// find the next newline character
	while(mFileStr.at(nodeSearchOffset) != '\n' && nodeSearchOffset < mFileStr.length())
		nodeSearchOffset++;

	// data parsing code line by line
	std::string dataParseStr;
	size_t dataParseOffset = nodeSearchOffset;
	while(nodeSearchOffset < mFileStr.length()){
		while((mFileStr.at(dataParseOffset + 1) != '\n')) dataParseOffset++;
		while(!isdigit(mFileStr.at(nodeSearchOffset)) && mFileStr.at(nodeSearchOffset) != '-') nodeSearchOffset++; // adjusts search offset properly

		dataParseStr = mFileStr.substr(nodeSearchOffset, dataParseOffset - nodeSearchOffset);
		if(dataParseStr.substr(0, mPosEndLabel.length()) == mPosEndLabel) break;
		else currentNode->appendPosExtractData(dataParseStr);

		nodeSearchOffset = dataParseOffset;
	}

	// TODO: Continue processing with other node attributes
}

Geo_Model3D File3D_Node::genRenderObj(){
    Geo_Model3D renderObj;
    // TODO: Populate render object with data

    return renderObj;
}