#include "Geo_Rect3D.hpp"

Eigen::Vector3f* Geo_Rect3D::genVertices() {
	if (getVertexData() != nullptr) cleanup();  // This means an attempt was made to override code
	// TODO: This code should be in the base class!!!!
	
    Eigen::Vector3f* data = (Eigen::Vector3f*)malloc(mVCount * sizeof(Eigen::Vector3f));
    
    // Front Face
    Eigen::Vector3f frontTopRight(mBox.height / 2.0f, mBox.width / 2.0f, DEFAULT_Z_VAL - (mBox.depth / 2.0f));
    Eigen::Vector3f frontTopLeft(mBox.height / 2.0f, -1 * (mBox.width / 2.0f), DEFAULT_Z_VAL - (mBox.depth / 2.0f));
    Eigen::Vector3f frontBotRight(-1 * (mBox.height / 2.0f), mBox.width / 2.0f, DEFAULT_Z_VAL - (mBox.depth / 2.0f));
    Eigen::Vector3f frontBotLeft(-1 * (mBox.height / 2.0f), -1 * (mBox.width / 2.0f), DEFAULT_Z_VAL - (mBox.depth / 2.0f)); 

    *(data + 0) = frontBotLeft;
    *(data + 1) = frontBotRight;
    *(data + 2) = frontTopLeft;
    *(data + 3) = frontTopRight;

    // Back Face
    Eigen::Vector3f backTopRight(mBox.height / 2.0f, mBox.width / 2.0f, DEFAULT_Z_VAL + (mBox.depth / 2.0f));
    Eigen::Vector3f backTopLeft(mBox.height / 2.0f, -1 * (mBox.width / 2.0f), DEFAULT_Z_VAL + (mBox.depth / 2.0f));
    Eigen::Vector3f backBotRight(-1 * (mBox.height / 2.0f), mBox.width / 2.0f, DEFAULT_Z_VAL + (mBox.depth / 2.0f));
    Eigen::Vector3f backBotLeft(-1 * (mBox.height / 2.0f), -1 * (mBox.width / 2.0f), DEFAULT_Z_VAL + (mBox.depth / 2.0f)); 

    *(data + 0) = backBotLeft;
    *(data + 1) = backBotRight;
    *(data + 2) = backTopLeft;
    *(data + 3) = backTopRight;

    return data;
}

Eigen::Vector2f* Geo_Rect3D::genTexCoords() {
	if (getTexCoordData() != nullptr) cleanup();  // This means an attempt was made to override code
	// TODO: This code should be in the base class!!!!

	Eigen::Vector2f* data = (Eigen::Vector2f*)malloc(mVCount * sizeof(Eigen::Vector2f));

	Eigen::Vector2f topRight(1.0f, 0.0f);
	Eigen::Vector2f topLeft(0.0f, 0.0f);
	Eigen::Vector2f botRight(1.0f, 1.0f);
	Eigen::Vector2f botLeft(0.0f, 1.0f);

    // Front Face
	*(data + 0) = botLeft;
	*(data + 1) = botRight;
	*(data + 2) = topLeft;
	*(data + 3) = topRight;

    // Back Face
	*(data + 4) = botLeft;
	*(data + 5) = botRight;
	*(data + 6) = topLeft;
	*(data + 7) = topRight;

	return data;
}

unsigned* Geo_Rect3D::genIndices(){
	if (getIndexData() != nullptr) cleanup();  // This means an attempt was made to override code
	// TODO: This code should be in the base class!!!!

    return nullptr; // No implementation
}
