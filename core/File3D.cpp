#include "File3D.hpp"

static std::string parseNodeName(const std::string& fileContent, size_t offset){
	if (offset > fileContent.size()) return std::string(); // safety check

	std::string name;
	while(!isalpha(fileContent.at(offset))) offset++; // search for next alpha character
	while (fileContent.at(offset) != '\n' && !isspace(fileContent.at(offset)) && fileContent.at(offset) != '\"') {
		name += fileContent.at(offset); // keep appending valid characters
		offset++;
	}
	for (unsigned c = 0; c < name.length(); c++) if (name[c] == '_') name[c] = '-'; // replace all underscores with dashes
	
	if(name == "default") return std::string(); // node cannot be named default because of obj parsing!
	else return name;
}

// Specific for OBJ files which include data denoted by a starting sequence
static std::string extractFileDataStr(const std::string& source, const std::string& sequence, unsigned nodeNum) {
	size_t startReadOffset = 0; size_t savedReadOffset; size_t endReadOffset = 0;
	bool matchSeqFound = false;
	unsigned nodeOffset = 0;
	std::string extractStr = "";

	while (source.find(sequence, startReadOffset) != std::string::npos & nodeOffset < nodeNum + 1) {
		startReadOffset = source.find(sequence, endReadOffset + 1);
		savedReadOffset = startReadOffset;
		endReadOffset = source.find('\n', savedReadOffset);
		extractStr = source.substr(savedReadOffset, endReadOffset - savedReadOffset);
		while (extractStr.substr(0, sequence.length()) == sequence) { // looks for correct sequence at the start of the line
			savedReadOffset = endReadOffset + 1;
			endReadOffset = source.find('\n', savedReadOffset);
			extractStr = source.substr(savedReadOffset, endReadOffset - savedReadOffset);
		}
		nodeOffset++;
	}

	extractStr = source.substr(startReadOffset, endReadOffset);
	return extractStr;
}

// Specific for DAE which separates data into xml tags
static std::string extractFileDataStr(const std::string& source, const tagTargets_t& tagTargets, unsigned nodeNum) {
	size_t startReadOffset = 0; size_t endReadOffset = 0;
	unsigned nodeOffset = 0;
	std::string openTagLabel = (tagTargets[INNER_LABEL_OFFSET] != "")? '<' + tagTargets[OPEN_TAG_OFFSET] : '<' + tagTargets[OPEN_TAG_OFFSET] + '>';
	std::string closeTagLabel = (tagTargets[INNER_LABEL_OFFSET] != "")? "<\/" + tagTargets[CLOSE_TAG_OFFSET] : "<\/" + tagTargets[CLOSE_TAG_OFFSET] + '>';
	std::string extractStr = "";

	while (source.find(openTagLabel, startReadOffset) != std::string::npos && nodeOffset < nodeNum + 1) {
		startReadOffset = openTagLabel.length() + source.find(openTagLabel, startReadOffset); // update startReadOffset to next position
		endReadOffset = startReadOffset;
		// Check for matching inner label
		if(tagTargets[INNER_LABEL_OFFSET] != ""){
			while (source[endReadOffset] != '\n') endReadOffset++; // advance until newline character encountered only if an inner label is encountered

			extractStr = source.substr(startReadOffset, endReadOffset);
			if (extractStr.find(tagTargets[INNER_LABEL_OFFSET]) == std::string::npos) continue; // match not encountered
		}
		nodeOffset++;
	}
	
	startReadOffset = endReadOffset + 1; // begin after the newline character of the proper tag
	endReadOffset = source.find(closeTagLabel, startReadOffset);
	extractStr = source.substr(startReadOffset, endReadOffset);

	return extractStr;
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

	std::string posDataStr;
	std::string normalsDataStr;
	std::string texCoordDataStr;
	std::string indexDataStr;
	File3D_Node* currentNode = *(mNodeData + nodeNum); // provides add data to the target node

	if (mParseType == F3D_CheckLine) { // used for parsing data based on start of line, see F3D_CheckLine enumeration
		posDataStr = extractFileDataStr(mFileSource, mLineParseAttribs.posData_seq, nodeNum);
		normalsDataStr = extractFileDataStr(mFileSource, mLineParseAttribs.normalsData_seq, nodeNum);
		texCoordDataStr = extractFileDataStr(mFileSource, mLineParseAttribs.texCoordData_seq, nodeNum);
		indexDataStr = extractFileDataStr(mFileSource, mLineParseAttribs.indexData_seq, nodeNum);
	}
	else if (mParseType == F3D_CheckTag) { // used for parsing data based on xml tags, see F3D_CheckTag enumeration
		posDataStr = extractFileDataStr(mFileSource, mTagParseAttribs.posData_tags, nodeNum);
		normalsDataStr = extractFileDataStr(mFileSource, mTagParseAttribs.normalsData_tags, nodeNum);
		texCoordDataStr = extractFileDataStr(mFileSource, mTagParseAttribs.texCoordData_tags, nodeNum);
		indexDataStr = extractFileDataStr(mFileSource, mTagParseAttribs.indexData_tags, nodeNum);
	}

	cleanupNumStr(&posDataStr); cleanupNumStr(&normalsDataStr); cleanupNumStr(&texCoordDataStr); cleanupNumStr(&indexDataStr);

	if (!posDataStr.empty()) currentNode->assignPosData(posDataStr);
	if (!normalsDataStr.empty()) currentNode->assignNormalsData(normalsDataStr);
	if (!texCoordDataStr.empty()) currentNode->assignTexCoordData(texCoordDataStr);
	if (!indexDataStr.empty()) currentNode->assignIndexData(indexDataStr);
}

/* Geo_Model3D File3D_Node::genRenderObj(){
    Geo_Model3D renderObj = Geo_Model3D();
    // TODO: Populate render object with data

    return renderObj;
} */