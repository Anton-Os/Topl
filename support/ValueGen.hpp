#ifndef VALUEGEN_H

#include <vector>
#include <algorithm>

#include "maths.h"

// Data Operations

typedef std::vector<uint8_t> blockBytes_t; // container used to pass data to shaders
typedef const uint8_t* bytes_cptr; // intermediate type for all shader data

#define NO_PADDING 0
#define PADDING_WIDTH 16 // padding aligned by 4 byte boundaries

void assignDataToBytes(bytes_cptr data, size_t dataSize, blockBytes_t* bytes); // overwrites all data
void alignDataToBytes(bytes_cptr data, size_t dataSize, size_t paddingSize, blockBytes_t* bytes); // aligns data, manual padding
void appendDataToBytes(bytes_cptr data, size_t dataSize, blockBytes_t* bytes); // appends data, computed 16-bit padding

// Transformation Operations

#define PPROJ_A -1.0
#define PPROJ_Z 5.0

enum PROJECTION_Type {
    PROJECTION_None,
    PROJECTION_Ortho,
    PROJECTION_Perspective,
    PROJECTION_Hyperbolic,
    PROJECTION_Experimental,
};

struct Projection { // Used in Matrix calculations
    Projection(){}
    Projection(PROJECTION_Type p){ type = p; }
    Projection(PROJECTION_Type p, float scaleFactor);
    Projection(PROJECTION_Type p, float l, float r, float b, float t, float n, float f);

    PROJECTION_Type type = PROJECTION_None;
    float left = -1.0f; float right = 1.0f;
    float bottom = -1.0f; float top = 1.0f;
    float nearPlane = -1.0f; float farPlane = 1.0f;

    Mat4x4 genProjMatrix();
};

// Mat4x4 genProjMatrix(const Projection& proj);

#define VALUEGEN_H
#endif