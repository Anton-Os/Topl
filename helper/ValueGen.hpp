#ifndef VALUEGEN_H

#include <vector>
#include <algorithm>

#include "Maths.h"

// Memory Operations

typedef std::vector<uint8_t> blockBytes_t; // container used to pass data to shaders
typedef const uint8_t* bytes_cptr; // intermediate type for all shader data

#define NO_PADDING 0
#define PADDING_WIDTH 16 // padding aligned by 4 byte boundaries

void assignDataToBytes(bytes_cptr data, size_t dataSize, blockBytes_t* bytes); // overwrites all data inside bytes
void appendDataToBytes(bytes_cptr data, size_t dataSize, blockBytes_t* bytes); // computed padding appended
void appendDataToBytes(bytes_cptr data, size_t dataSize, size_t paddingSize, blockBytes_t* bytes); // manual padding

// Mathematic Operations

enum PROJECTION_Type {
    PROJECTION_Ortho,
    PROJECTION_Perspective,
    PROJECTION_Stereo,
    PROJECTION_Gnomonic
};

struct SpatialBounds3D { // Used in Matrix calculations
    SpatialBounds3D(){}
    SpatialBounds3D(float scaleFactor);
    SpatialBounds3D(float l, float r, float b, float t, float n, float f);
    
    float left = -1.0f; float right = 1.0f;
    float bottom = -1.0f; float top = 1.0f;
    float nearPlane = 0; float farPlane = 1.0f;
};

Mat4x4 genProjMatrix(PROJECTION_Type type, const SpatialBounds3D& bounds);
unsigned genRandColor();

#define VALUEGEN_H
#endif