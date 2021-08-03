#include "Geo_ModelNode.hpp"

void Geo_ModelNode::genVertices(Eigen::Vector3f* data){
	// TODO: Implement body here
	return;
}

void Geo_ModelNode::genNormals(Eigen::Vector3f* data) {
	// TODO: Implement body here
	return;
}

void Geo_ModelNode::genTexCoords(Eigen::Vector2f* data) {
	// TODO: Implement body here
	return;
}

void Geo_ModelNode::genIndices(unsigned* data){
	// TODO: Implement body here
	return;
}

void Geo_ModelNode::log(const char* fileName) { // for debugging
	std::string currentLine;

	logToFile(fileName, "\n\nPos Data\n");
	// logToFile(fileName, *(mNode->getPosDataStr()));

	logToFile(fileName, "\n\nIndex Data\n");
	// logToFile(fileName, *(mNode->getIndexDataStr()));
}