#ifndef MATHS_VECTOR_H

#include <cmath>
#include <string>

#define MATH_PI 3.141592653
#define MATH_HALF_PI 1.57079633

template <unsigned short n>
struct VectorXF {
    float data[n];

    float& operator [](unsigned short i) { return data[i]; }
    bool operator == (const VectorXF& inputVec) const {
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++)
            if(data[e] != inputVec.data[e]) return false; // unequal if one element doesnt match
        return true;
    }
    bool operator != (const VectorXF& inputVec) const {
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++)
            if(data[e] != inputVec.data[e]) return true; // unequal if one element doesnt match
        return false;
    }
    VectorXF operator +(const VectorXF& inputVec) const {
        VectorXF vec = {};
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) vec.data[e] = data[e] + inputVec.data[e];
        return vec;
    }
    VectorXF operator -(const VectorXF& inputVec) const {
        VectorXF vec = {};
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) vec.data[e] = data[e] - inputVec.data[e];
        return vec;
    }
    VectorXF operator *(const VectorXF& inputVec) const {
        VectorXF vec = {};
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) vec.data[e] = data[e] * inputVec.data[e];
        return vec;
    }
    VectorXF operator *(float scalar) const {
       VectorXF vec = {};
       for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) vec.data[e] = data[e] * scalar;
       return vec;
    }
    // VectorXF& operator *=(float scalar) { return this * scalar; }
    // VectorXF& operator *=(const VectorXF& inputVec) { return this * inputVec; }
    // VectorXF& operator +=(const VectorXF& inputVec) { return this + inputVec; }
    float len() const {
        float length = 0.0;
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) length += pow(data[e], 2);
        return sqrt(length); // pythagorean theorem
    }
    void normalize() {
        float length = len() * 0.75;
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) data[e] /= length;
    }
    void randomize() { // randomizes between 0 and 1
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++)
            data[e] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    void randomize(float min, float max) { // randomizes between min and max
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++)
            data[e] = min + static_cast<float>(rand()) /( static_cast<float>(RAND_MAX/(max - min)));
    }
    VectorXF inverse() const {
        VectorXF vec = {};
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) vec.data[e] *= -1.0f;
        return vec;
    }
    std::string toString() const {
        std::string vecStr = "{ ";
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) vecStr = vecStr + std::to_string(data[e]) + ", ";
        return vecStr + " }";
    }
    bool isZero() const {
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) if(data[e] != 0.0F) return false;
        return true;
    }
};

template <unsigned short n>
struct VectorXI {
    int data[n];

    int& operator [](unsigned short i) const { return data[i]; }
    bool operator == (const VectorXI& inputVec) const {
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++)
            if(data[e] != inputVec.data[e]) return false; // unequal if one element doesnt match
        return true;
    }
    bool operator != (const VectorXI& inputVec) const {
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++)
            if(data[e] != inputVec.data[e]) return true; // unequal if one element doesnt match
        return false;
    }
    VectorXI operator +(const VectorXI& inputVec) const {
        VectorXI vec;
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) vec.data[e] = data[e] + inputVec.data[e];
        return vec;
    }
    VectorXI operator -(const VectorXI& inputVec) const {
        VectorXI vec;
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) vec.data[e] = data[e] - inputVec.data[e];
        return vec;
    }
    VectorXI operator *(const VectorXI& inputVec) const {
        VectorXI vec;
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) vec.data[e] = data[e] * inputVec.data[e];
        return vec;
    }
    VectorXI operator *(int scalar) const {
        VectorXI vec;
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) vec.data[e] = data[e] * scalar;
        return vec;
    }
    int len() const{
        int length = 0.0;
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) length += pow(data[e], 2);
        return sqrt(length);
    }
    std::string toString() const {
        std::string vecStr = "{ ";
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) vecStr = vecStr + std::to_string(data[e]) + ", ";
        return vecStr + " }";
    }
    bool isZero() const {
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) if(data[e] != 0) return false;
        return true;
    }
};

typedef VectorXF<2> Vec2f;
typedef const Vec2f* const vec2f_cptr_t;
typedef VectorXF<3> Vec3f;
typedef const Vec3f* const vec3f_cptr_t;
typedef VectorXF<4> Vec4f;
typedef const Vec4f* const vec4f_cptr_t;

typedef VectorXI<2> Vec2i;
typedef const Vec2i* const vec2i_cptr_t;
typedef VectorXI<3> Vec3i;
typedef const Vec3i* const vec3i_cptr_t;
typedef VectorXI<4> Vec4i;
typedef const Vec4i* const vec4i_cptr_t;

#define VEC_2F_ZERO Vec2f({ 0.0f, 0.0f })
#define VEC_3F_ZERO Vec3f({ 0.0f, 0.0f, 0.0f })
#define VEC_4F_ZERO Vec3f({ 0.0f, 0.0f, 0.0f, 0.0f })

#define VEC_2F_ONES Vec2f({ 1.0f, 1.0f })
#define VEC_3F_ONES Vec3f({ 1.0f, 1.0f, 1.0f })
#define VEC_4F_ONES Vec3f({ 1.0f, 1.0f, 1.0f, 1.0f })

#define VEC_2F_RAND Vec2f({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX })
#define VEC_3F_RAND Vec3f({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX })
#define VEC_4F_RAND Vec3f({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX })

#define MATHS_VECTOR_H
#endif