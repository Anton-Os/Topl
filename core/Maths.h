#include <cmath>

template <unsigned short n>
struct VectorXF { 
    float data[n];
    float operator [](unsigned short i){ return data[i]; }
    VectorXF operator +(const VectorXF& vector){ 
        unsigned size = sizeof(data) / sizeof(float);
        for(unsigned e = 0; e < size; e++) data[e] += vector.data[e];
    }
    VectorXF operator -(const VectorXF& vector){ 
        unsigned size = sizeof(data) / sizeof(float);
        for(unsigned e = 0; e < size; e++) data[e] -= vector.data[e];
    }
    VectorXF operator *(const VectorXF& vector){ 
        unsigned size = sizeof(data) / sizeof(float);
        for(unsigned e = 0; e < size; e++) data[e] *= vector.data[e];
    }
    void norm(){
        unsigned size = sizeof(data) / sizeof(float);
        float length = 0.0;
        for(unsigned e = 0; e < size; e++) length += pow(data[e], 2);
        length = sqrt(length); // pythagor theorem
        for(unsigned e = 0; e < size; e++) data[e] /= length;
    }
};

template <unsigned short n>
struct VectorXI { 
    int data[n];
    int operator [](unsigned short i){ return data[i]; }
    VectorXI operator +(const VectorXI& vector){ 
        unsigned size = sizeof(data) / sizeof(float);
        for(unsigned e = 0; e < size; e++) data[e] += vector.data[e];
    }
    VectorXI operator -(const VectorXI& vector){ 
        unsigned size = sizeof(data) / sizeof(float);
        for(unsigned e = 0; e < size; e++) data[e] -= vector.data[e];
    }
    VectorXI operator *(const VectorXI& vector){ 
        unsigned size = sizeof(data) / sizeof(float);
        for(unsigned e = 0; e < size; e++) data[e] *= vector.data[e];
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
typedef VectorXF<3> Vec3f;
typedef VectorXF<4> Vec4f;

typedef VectorXI<2> Vec2i;
typedef VectorXI<3> Vec3i;
typedef VectorXI<4> Vec4i;

typedef Matrix<2, 2> Mat2x2;
typedef Matrix<3, 3> Mat3x3;
typedef Matrix<4, 4> Mat4x4;