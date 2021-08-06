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

void ValueGen::appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes){
    size_t paddingSize = 4 - (dataSize % 4); // manually computed padding value
    appendDataToBytes(data_ptr, dataSize, paddingSize, targetBytes);
}

void ValueGen::appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingSize, std::vector<uint8_t>* targetBytes){
    for(unsigned d = 0; d < dataSize + paddingSize; d++)
        (d < dataSize && data_ptr + d != nullptr)
            ? targetBytes->push_back(*(data_ptr + d)) // value is copied into targetBytes
            : targetBytes->push_back(0); // otherwise zero padding is applied
}

void ValueGen::assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes){
    targetBytes->clear();
    targetBytes->resize(dataSize);
    
    for(unsigned d = 0; d < dataSize; d++)
        if(data_ptr + d != nullptr) targetBytes->at(d) = *(data_ptr + d);
}