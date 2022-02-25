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
    float left = l;
    float right = r;
    float bottom = b;
    float top = t;
    float nearPlane = n;
    float farPlane = f;
}

// Memory Operations

void appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes){
    size_t paddingSize = PADDING_WIDTH - (dataSize % PADDING_WIDTH); // manually computed padding value
    appendDataToBytes(data_ptr, dataSize, paddingSize, targetBytes);
}

void appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingSize, std::vector<uint8_t>* targetBytes){
    for(unsigned d = 0; d < dataSize + paddingSize; d++)
        (d < dataSize && data_ptr + d != nullptr)
            ? targetBytes->push_back(*(data_ptr + d)) // value is copied into targetBytes
            : targetBytes->push_back(0); // otherwise zero padding is applied
}

void assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes){
    targetBytes->clear();
    targetBytes->resize(dataSize);
    
    for(unsigned d = 0; d < dataSize; d++)
        if(data_ptr + d != nullptr) targetBytes->at(d) = *(data_ptr + d);
}

// Mathematic Operations

Eigen::Matrix4f genPerspectiveMatrix(SpatialBounds3D bounds){
    Eigen::Matrix4f projMatrix;
    projMatrix << // From OpenGL SuperBible starting page 86
        2.0f / (bounds.right - bounds.left), 0.0f, (bounds.right + bounds.left) / (bounds.right - bounds.left), 0.0f,
        0.0f, (2.0f * bounds.nearPlane) / (bounds.top - bounds.bottom), (bounds.top + bounds.bottom) / (bounds.top - bounds.bottom), 0.0f,
        0.0f, 0.0f, (bounds.nearPlane + bounds.farPlane) / (bounds.nearPlane - bounds.farPlane), (2.0f * bounds.nearPlane * bounds.farPlane) / (bounds.nearPlane - bounds.farPlane),
        0.0f, 0.0f, -1.0f, 0.0f;
    return projMatrix;
}

Eigen::Matrix4f genOrthoMatrix(SpatialBounds3D bounds){
    Eigen::Matrix4f projMatrix;
    projMatrix << // From OpenGL SuperBible starting page 86
        2.0f / (bounds.right - bounds.left), 0.0f, 0.0f, (bounds.left + bounds.right) / (bounds.left - bounds.right),
        0.0f, 2.0f / (bounds.top - bounds.bottom), 0.0f, (bounds.bottom + bounds.top) / (bounds.bottom - bounds.top),
        0.0f, 0.0f, 2.0f / (bounds.nearPlane - bounds.farPlane), (bounds.farPlane + bounds.nearPlane) / (bounds.farPlane - bounds.nearPlane),
        0.0f, 0.0f, 0.0f, 1.0f;
    return projMatrix;
}

unsigned genRandColor(){
    uint8_t redBit = rand() % 255;
	uint8_t greenBit = rand() % 255;
	uint8_t blueBit = rand() % 255;
	return (uint32_t)((0xFF << 24) + (redBit << 16) + (greenBit << 8) + blueBit);
}

float genRandFloat(){ return genRandFloat(0.0, 1.0); }
float genRandFloat(float min, float max){ return min + static_cast<float>(rand()) /( static_cast<float>(RAND_MAX/(max - min))); }
float getVecLength(const Eigen::Vector2f& vec){ return sqrt(pow(vec.x(), 2) + pow(vec.y(), 2)); }
float getVecLength(const Eigen::Vector3f& vec){ return sqrt(pow(vec.x(), 2) + pow(vec.y(), 2) + pow(vec.z(), 2)); }
Eigen::Vector2f genRandVec2(){ return Eigen::Vector2f(genRandFloat(), genRandFloat()); }
Eigen::Vector3f genRandVec3(){ return Eigen::Vector3f(genRandFloat(), genRandFloat(), genRandFloat()); }
Eigen::Vector4f genRandVec4(){ return Eigen::Vector4f(genRandFloat(), genRandFloat(), genRandFloat(), genRandFloat()); }