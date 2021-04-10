#include "Geo_Box3D.hpp"

void Geo_Box3D::genVertices(Eigen::Vector3f* data) {
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

    *(data + 4) = backBotLeft;
    *(data + 5) = backBotRight;
    *(data + 6) = backTopLeft;
    *(data + 7) = backTopRight;
}

void Geo_Box3D::genTexCoords(Eigen::Vector2f* data) {
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
}

void Geo_Box3D::genIndices(unsigned* data){
    return; // No implementation
}
