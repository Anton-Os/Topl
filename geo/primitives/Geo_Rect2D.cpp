#include "Geo_Rect2D.hpp"

Eigen::Vector3f* Geo_Rect2D::genVertices() {
	if (getVData() != nullptr) cleanup();  // This means an attempt was made to override code
	// TODO: This code should be in the base class!!!!
	
    Eigen::Vector3f* data = (Eigen::Vector3f*)malloc(mVCount * sizeof(Eigen::Vector3f));
    
    // The depth position of the vector is arbitrary for now
    Eigen::Vector3f topRight(mRect.height / 2.0f, mRect.width / 2.0f, DEFAULT_Z_VAL);
    Eigen::Vector3f topLeft(mRect.height / 2.0f, -1 * (mRect.width / 2.0f), DEFAULT_Z_VAL);
    Eigen::Vector3f botRight(-1 * (mRect.height / 2.0f), mRect.width / 2.0f, DEFAULT_Z_VAL);
    Eigen::Vector3f botLeft(-1 * (mRect.height / 2.0f), -1 * (mRect.width / 2.0f), DEFAULT_Z_VAL); 

    *(data + 0) = botLeft;
    *(data + 1) = botRight;
    *(data + 2) = topLeft;
    *(data + 3) = topRight;

    return data; // FIX! FIX! FIX!
}

Eigen::Vector2f* Geo_Rect2D::genTexCoords() {
	if (getTData() != nullptr) cleanup();  // This means an attempt was made to override code
	// TODO: This code should be in the base class!!!!

	Eigen::Vector2f* data = (Eigen::Vector2f*)malloc(mVCount * sizeof(Eigen::Vector2f));

	Eigen::Vector2f topRight(1.0f, 0.0f);
	Eigen::Vector2f topLeft(0.0f, 0.0f);
	Eigen::Vector2f botRight(1.0f, 1.0f);
	Eigen::Vector2f botLeft(0.0f, 1.0f);

	*(data + 0) = botLeft;
	*(data + 1) = botRight;
	*(data + 2) = topLeft;
	*(data + 3) = topRight;

	return data;
}

unsigned* Geo_Rect2D::genIndices(){
	if (getIData() != nullptr) cleanup();  // This means an attempt was made to override code
	// TODO: This code should be in the base class!!!!

    unsigned* data = (unsigned*)malloc(mICount * sizeof(unsigned));

    unsigned indicesArray[6] = { 
        0, 1, 2, 
        3, 2, 1
    };

    // Simply copies from array to to the allocated data
    for(unsigned i = 0; i < 6; i++)
        *(data + i) = indicesArray[i];

    return data;
}
