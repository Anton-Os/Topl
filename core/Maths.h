#ifndef MATHS_H

#include <cmath>

#define TOPL_PI 3.141592653
#define TOPL_HALF_PI 1.57079633

template <unsigned short n>
struct VectorXF { 
    float data[n];

    float operator [](unsigned short i){ return data[i]; }
    VectorXF operator +(const VectorXF& vector){ 
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) data[e] += vector.data[e];
    }
    VectorXF operator -(const VectorXF& vector){ 
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) data[e] -= vector.data[e];
    }
    VectorXF operator *(const VectorXF& vector){ 
       for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) data[e] *= vector.data[e];
    }
    float len(){
        float length = 0.0;
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) length += pow(data[e], 2);
        return sqrt(length); // pythagorean theorem
    }
    VectorXF norm(){
        VectorXF normVec;
        float length = len();
        for(unsigned e = 0; e < sizeof(data) / sizeof(float); e++) normVec.data[e] = data[e] / length;
    }
};

template <unsigned short n>
struct VectorXI { 
    int data[n];

    int operator [](unsigned short i){ return data[i]; }
    VectorXI operator +(const VectorXI& vector){ 
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) data[e] += vector.data[e];
    }
    VectorXI operator -(const VectorXI& vector){ 
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) data[e] -= vector.data[e];
    }
    VectorXI operator *(const VectorXI& vector){ 
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) data[e] *= vector.data[e];
    }
    int len(){
        int length = 0.0;
        for(unsigned e = 0; e < sizeof(data) / sizeof(int); e++) length += pow(data[e], 2);
        return sqrt(length);
    }
};

template <unsigned short r, unsigned short c>
struct Matrix {
	float data[r][c];
	float operator () (unsigned short r, unsigned short c) {
		return data[r][c];
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

typedef Matrix<2, 2> Mat2x2;
typedef const Mat2x2* const mat2x2_cptr_t;
typedef Matrix<3, 3> Mat3x3;
typedef const Mat3x3* const mat3x3_cptr_t;
typedef Matrix<4, 4> Mat4x4;
typedef const Mat4x4* const mat4x4_cptr_t;

#define MATHS_H
#endif