#ifndef VALUEGEN_H

#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <vector>

#include <Eigen/Dense>

#define PADDING_WIDTH 16 // padding should be aligned by 4 byte boundaries

enum PROJECTION_Type {
    PROJECTION_Perspective,
    PROJECTION_Ortho
};

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
    static float genRandFloat(){ return genRandFloat(0.0, 1.0); }
    static float genRandFloat(float min, float max){ return min + static_cast<float>(rand()) /( static_cast<float>(RAND_MAX/(max - min))); }
    static Eigen::Vector2f genRandVec2(){ return Eigen::Vector2f(genRandFloat(), genRandFloat()); }
    static Eigen::Vector3f genRandVec3(){ return Eigen::Vector3f(genRandFloat(), genRandFloat(), genRandFloat()); }
    static Eigen::Vector4f genRandVec4(){ return Eigen::Vector4f(genRandFloat(), genRandFloat(), genRandFloat(), genRandFloat()); }

    static void appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes); // default padding
    static void appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingSize, std::vector<uint8_t>* targetBytes); // custom padding
    static void assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes);
    // void assignDataToBytes(void* dataPtr, std::vector<uint8_t>* bytes);
};

#define VALUEGEN_H
#endif