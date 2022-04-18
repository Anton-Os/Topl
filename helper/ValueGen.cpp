#include "ValueGen.hpp"

SpatialBounds3D::SpatialBounds3D(float scaleFactor){
    left *= scaleFactor;
    right *= scaleFactor;
    bottom *= scaleFactor;
    top *= scaleFactor;
    nearPlane *= scaleFactor;
    farPlane *= scaleFactor;
}

SpatialBounds3D::SpatialBounds3D(float l, float r, float b, float t, float n, float f){
    left = l; 
    right = r;
    bottom = b; 
    top = t;
    nearPlane = n; 
    farPlane = f;
}

// Memory Operations

void assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes){
    targetBytes->clear();
    targetBytes->resize(dataSize);
    
    for(unsigned d = 0; d < dataSize; d++)
        if(data_ptr + d != nullptr) targetBytes->at(d) = *(data_ptr + d);
}

void alignDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingSize, std::vector<uint8_t>* targetBytes) {
	for (unsigned d = 0; d < dataSize + paddingSize; d++)
		(d < dataSize && data_ptr + d != nullptr)
		? targetBytes->push_back(*(data_ptr + d)) // value is copied into targetBytes
		: targetBytes->push_back(0); // otherwise zero padding is applied
}

void appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes){
    size_t paddingSize = PADDING_WIDTH - (dataSize % PADDING_WIDTH); // manually computed padding value
    alignDataToBytes(data_ptr, dataSize, paddingSize, targetBytes);
}

// Mathematic Operations

static Mat4x4 genPerspectiveMatrix(SpatialBounds3D bounds){
    Mat4x4 projMatrix = Mat4x4({ // From OpenGL SuperBible starting page 88
        (2.0f * bounds.nearPlane) / (bounds.right - bounds.left), 0.0f, (bounds.right + bounds.left) / (bounds.right - bounds.left), 0.0f,
        0.0f, (2.0f * bounds.nearPlane) / (bounds.top - bounds.bottom), (bounds.top + bounds.bottom) / (bounds.top - bounds.bottom), 0.0f,
        0.0f, 0.0f, (bounds.nearPlane + bounds.farPlane) / (bounds.nearPlane - bounds.farPlane), (2.0f * bounds.nearPlane * bounds.farPlane) / (bounds.nearPlane - bounds.farPlane),
        0.0f, 0.0f, -1.0f, 0.0f
    });
    return projMatrix;
}

static Mat4x4 genOrthoMatrix(SpatialBounds3D bounds){
    Mat4x4 projMatrix = Mat4x4({ // From OpenGL SuperBible starting page 89
        2.0f / (bounds.right - bounds.left), 0.0f, 0.0f, (bounds.left + bounds.right) / (bounds.left - bounds.right),
        0.0f, 2.0f / (bounds.top - bounds.bottom), 0.0f, (bounds.bottom + bounds.top) / (bounds.bottom - bounds.top),
        0.0f, 0.0f, 2.0f / (bounds.nearPlane - bounds.farPlane), (bounds.farPlane + bounds.nearPlane) / (bounds.farPlane - bounds.nearPlane),
        0.0f, 0.0f, 0.0f, 1.0f });
    return projMatrix;
}

static Mat4x4 genStereoMatrix(SpatialBounds3D bounds){
    return MAT_4x4_IDENTITY;
}

static Mat4x4 genGnomonicMatrix(SpatialBounds3D bounds){
    return MAT_4x4_IDENTITY;
}

Mat4x4 genProjMatrix(PROJECTION_Type type, const SpatialBounds3D& bounds){
    switch(type){
        case PROJECTION_Perspective: return genPerspectiveMatrix(bounds);
        case PROJECTION_Ortho: return genOrthoMatrix(bounds);
        case PROJECTION_Stereo: return genStereoMatrix(bounds);
        case PROJECTION_Gnomonic: return genGnomonicMatrix(bounds);
        // case PROJECTION_Test: return MAT_4x4_TEST;
    }
}

unsigned genRandColor(){
    uint8_t redBit = rand() % 255;
	uint8_t greenBit = rand() % 255;
	uint8_t blueBit = rand() % 255;
	return (uint32_t)((0xFF << 24) + (redBit << 16) + (greenBit << 8) + blueBit);
}