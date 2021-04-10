#include "ValueGen.hpp"

Eigen::Matrix4f ValueGen::genPerspectiveMatrix(SpatialBounds3D bounds){
    Eigen::Matrix4f projMatrix;
    projMatrix << // From OpenGL SuperBible starting page 86
        2.0f / (bounds.right - bounds.left), 0.0f, (bounds.right + bounds.left) / (bounds.right - bounds.left), 0.0f,
        0.0f, (2.0f * bounds.nearPlane) / (bounds.top - bounds.bottom), (bounds.top + bounds.bottom) / (bounds.top - bounds.bottom), 0.0f,
        0.0f, 0.0f, (bounds.nearPlane + bounds.farPlane) / (bounds.nearPlane - bounds.farPlane), (2.0f * bounds.nearPlane * bounds.farPlane) / (bounds.nearPlane - bounds.farPlane),
        0.0f, 0.0f, -1.0f, 0.0f;
    return projMatrix;
}

Eigen::Matrix4f ValueGen::genOrthoMatrix(SpatialBounds3D bounds){
    Eigen::Matrix4f projMatrix;
    projMatrix << // From OpenGL SuperBible starting page 86
        2.0f / (bounds.right - bounds.left), 0.0f, 0.0f, (bounds.left + bounds.right) / (bounds.left - bounds.right),
        0.0f, 2.0f / (bounds.top - bounds.bottom), 0.0f, (bounds.bottom + bounds.top) / (bounds.bottom - bounds.top),
        0.0f, 0.0f, 2.0f / (bounds.nearPlane - bounds.farPlane), (bounds.farPlane + bounds.nearPlane) / (bounds.farPlane - bounds.nearPlane),
        0.0f, 0.0f, 0.0f, 1.0f;
    return projMatrix;
}

void ValueGen::appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingByteCount, std::vector<uint8_t>* bytes){
    for(unsigned d = 0; d < dataSize + paddingByteCount; d++)
        (d < dataSize && data_ptr + d != nullptr)
            ? bytes->push_back(*(data_ptr + d)) // Value is copied into bytes
            : bytes->push_back(0); // Otherwise padding is created
}

void ValueGen::assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* bytes){
    bytes->clear();
    bytes->resize(dataSize);
    
    for(unsigned d = 0; d < dataSize; d++)
        if(data_ptr + d != nullptr) bytes->at(d) = *(data_ptr + d);
}