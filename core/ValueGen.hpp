#ifndef VALUEGEN_H

#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <vector>

#include <Eigen/Dense>

struct SpatialBounds3D { // Used in Matrix calculations
    SpatialBounds3D(){}
    SpatialBounds3D(float scaleFactor){
        left *= scaleFactor;
        right *= scaleFactor;
        bottom *= scaleFactor;
        top *= scaleFactor;
        nearPlane *= scaleFactor;
        farPlane *= scaleFactor;
    }
    float left = -1.0f;
    float right = 1.0f;
    float bottom = -1.0f;
    float top = 1.0f;
    float nearPlane = 0;
    float farPlane = 1.0f;
};

struct ValueGen {
    ValueGen(){
        srand(time(NULL)); // random value seeder
    }

    static Eigen::Matrix4f genPerspectiveMatrix(SpatialBounds3D bounds);
    static Eigen::Matrix4f genOrthoMatrix(SpatialBounds3D bounds);
    static float genRandFloat(){ genRandFloat(0.0, 1.0); }
    static float genRandFloat(float minBound, float maxBound);

    static void appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingByteCount, std::vector<uint8_t>* bytes); // Implement later
    static void assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* bytes);
    // void assignDataToBytes(void* dataPtr, std::vector<uint8_t>* bytes);
};

#define VALUEGEN_H
#endif