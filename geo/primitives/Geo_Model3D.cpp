#include "Geo_Model3D.hpp"

void Geo_Model3D::genVertices(Eigen::Vector3f* data){
	unsigned vAttribCount = 0;
	unsigned vertexCount = 0;
	float x, y, z;
	Eigen::Vector3f currentVertex;
	std::string currentVal;
	bool isValReading = false;

	for (std::string::const_iterator ci = mNode->getPosDataStr()->cbegin(); ci < mNode->getPosDataStr()->cend(); ci++)
		if (vertexCount >= getVertexCount()) break; // safety escape mechanism
		else if (!isspace(*ci) && isValReading == false) {
			if (!currentVal.empty()) { // value cannot be empty
				// *(data + vAttribCount) = atoi(currentVal.c_str()); // pushes current value read
				if(vAttribCount % 3 == X_FILE_OFFSET) x = atof(currentVal.c_str()); // x value assignment for first string element
				else if(vAttribCount % 3 == Y_FILE_OFFSET) y = atof(currentVal.c_str()); // y value assignment for second string element
				else if(vAttribCount % 3 == Z_FILE_OFFSET) { // z is the last value so conclude data assignment
					z = atof(currentVal.c_str()); // z value assignment for third string element
					currentVertex = Eigen::Vector3f(x, y, z);
					*(data + vertexCount) = currentVertex;
					vertexCount++; // increment vertexCount separately!
				}
				currentVal.clear();
			}
			vAttribCount++;
			isValReading = true; // indicates that a value is being read
			currentVal += *ci;
		}
		else if (isspace(*ci) && isValReading) isValReading = false; // indicates that value is done reading
		else if (!isspace(*ci)) currentVal += *ci;
		else continue;

	return;
}

void Geo_Model3D::genNormals(Eigen::Vector3f* data) {
	// TODO: Implement body here
	return;
}

void Geo_Model3D::genTexCoords(Eigen::Vector2f* data) {
	// TODO: Implement body here
	return;
}

void Geo_Model3D::genIndices(unsigned* data){
	unsigned indexCount = 0;
	std::string currentVal;
	bool isValReading = false;

	for (std::string::const_iterator ci = mNode->getIndexDataStr()->cbegin(); ci < mNode->getIndexDataStr()->cend(); ci++)
		if (indexCount >= getIndexCount()) break; // safety escape mechanism
		else if (!isspace(*ci) && isValReading == false) {
			if (!currentVal.empty()) { // value cannot be empty
				*(data + indexCount) = atoi(currentVal.c_str()); // pushes current value read
				indexCount++;

				currentVal.clear();
			}
			isValReading = true; // indicates that a value is being read
			currentVal += *ci;
		}
		else if (isspace(*ci) && isValReading) isValReading = false; // indicates that value is done reading
		else if (!isspace(*ci)) currentVal += *ci;
		else continue;

	return;
}