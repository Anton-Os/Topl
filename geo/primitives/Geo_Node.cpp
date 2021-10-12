#include "Geo_Node.hpp"

void Geo_Node::genVertices(Eigen::Vector3f* data){
	// TODO: Implement body here
	return;
}

void Geo_Node::genNormals(Eigen::Vector3f* data) {
	// TODO: Implement body here
	return;
}

void Geo_Node::genTexCoords(Eigen::Vector2f* data) {
	// TODO: Implement body here
	return;
}

void Geo_Node::genIndices(unsigned* data){
	// TODO: Implement body here
	return;
}

void Geo_Node::log(const char* fileName) { // for debugging
	std::string currentLine;

	logToFile(fileName, "\n\nPos Data\n");
	// logToFile(fileName, *(mNode->getPosDataStr()));

	logToFile(fileName, "\n\nIndex Data\n");
	// logToFile(fileName, *(mNode->getIndicesStr()));
}