#ifndef VALUEGEN_H

// #include <ctime>
#include <cstdlib>
#include <cstdint>
#include <vector>

#include "Maths.h" // replace Eigen library!
#include <Eigen/Dense>

enum PROJECTION_Type {
    PROJECTION_Perspective,
    PROJECTION_Ortho
};

struct SpatialBounds3D { // Used in Matrix calculations
    SpatialBounds3D(){}
    SpatialBounds3D(float scaleFactor);
    SpatialBounds3D(float l, float r, float b, float t, float n, float f);
    
    float left = -1.0f;
    float right = 1.0f;
    float bottom = -1.0f;
    float top = 1.0f;
    float nearPlane = 0;
    float farPlane = 1.0f;
};

typedef std::vector<uint8_t> blockBytes_t; // format used for passing data to shaders
typedef const uint8_t* bytes_cptr;

// Memory Operations

#define PADDING_WIDTH 16 // padding should be aligned by 4 byte boundaries

void assignDataToBytes(bytes_cptr data_ptr, size_t dataSize, blockBytes_t* targetBytes);
void appendDataToBytes(bytes_cptr data_ptr, size_t dataSize, blockBytes_t* targetBytes); // default padding
void appendDataToBytes(bytes_cptr data_ptr, size_t dataSize, size_t paddingSize, blockBytes_t* targetBytes); // custom padding

// Mathematic Operations

Eigen::Matrix4f genPerspectiveMatrix(SpatialBounds3D bounds);
Eigen::Matrix4f genOrthoMatrix(SpatialBounds3D bounds);
Eigen::Matrix4f genHyperbolicMatrix(SpatialBounds3D bounds); // implement later
unsigned genRandColor();
float genRandFloat();
float genRandFloat(float min, float max);
float getVecLength(const Eigen::Vector2f& vec);
float getVecLength(const Eigen::Vector3f& vec);
Eigen::Vector2f genRandVec2();
Eigen::Vector3f genRandVec3();
Eigen::Vector4f genRandVec4();

#define VALUEGEN_H
#endif