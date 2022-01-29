#ifndef GEOMETRY_H

#include <cstdlib>
#include <cmath>

#include "Physics.h" // added to generate collision objects

#define DEFAULT_CIRCLE_SEGS 1001 // this is a chiliagon + 1 for odd sides
#define TOPL_PI 3.141592653
#define TOPL_HALF_PI 1.57079633
#include <Eigen/Dense> // Examine documentation

#define POSITION_COUNT 3
#define NORMALS_COUNT 3
#define TEXCOORD_COUNT 2
// #define BLENDWEIGHTS_COUNT 3

// #define DEFAULT_Z_VAL 0.1f
#define DEFAULT_Z_VAL 0.0f
#define X_OFFSET 0
#define Y_OFFSET 1
#define Z_OFFSET 2
#define U_OFFSET 0
#define V_OFFSET 1

// Typedefs for const safe types
typedef const Eigen::Vector3f* const vec3f_cptr;
typedef const Eigen::Vector2f* const vec2f_cptr;
typedef const Eigen::Matrix4f* const mat4f_cptr;
typedef const Eigen::Matrix3f* const mat3f_cptr;
typedef const Eigen::Matrix2f* const mat2f_cptr;
typedef const unsigned* const ui_cptr;

struct Geo_Vertex {
	Geo_Vertex(Eigen::Vector3f pos); // Position data constructor
	Geo_Vertex(Eigen::Vector3f pos, Eigen::Vector2f texc); // Extended constructor

	float position[POSITION_COUNT];
	float texCoord[TEXCOORD_COUNT];
	// float normals[NORMALS_COUNT]; // might need to implement
};

typedef const Geo_Vertex* const vertex_cptr; // Safe const pointer type

struct NGon2D {
    float radius;
    unsigned short segments;
};

struct NGon3D {
	float radius;
	unsigned short xSegments;
	unsigned short ySegments;
};

enum AXIS_Target{
    AXIS_X,
    AXIS_Y,
    AXIS_Z
};

typedef float (*vTformCallback)(float, double); // callback for transforming vertex attributes given input and modifier arguments

class Geo_RenderObj {
public:
	Geo_RenderObj(){} // empty constructor
	Geo_RenderObj(unsigned v); // vertex only constructor
    Geo_RenderObj(unsigned v, unsigned i); // vertex and indices constructor
    
	~Geo_RenderObj(){ cleanup(); }
	void cleanup();
	void modify(vTformCallback callback, double mod, AXIS_Target axis);

    unsigned getVerticesCount() const { return _verticesCount; } // get Vertex Count
    unsigned getIndexCount() const { return _indicesCount; } // get Index Count

	vertex_cptr getVertices();
    ui_cptr getIndices() const { return _indices; }
	vec3f_cptr getPosData() const { return _posData; }
	vec3f_cptr getNormalsData() const { return _normalsData; }
    vec2f_cptr getTexCoordData() const { return _texCoordData; }
protected:
	void fillRenderObj();
    virtual void genVertices(Eigen::Vector3f* data) = 0;
	virtual void genNormals(Eigen::Vector3f* data) = 0;
    virtual void genTexCoords(Eigen::Vector2f* data) = 0;
    virtual void genIndices(unsigned* data) = 0;

	double _startAngle = 0.0; // starting angle for vertex generation

    unsigned _verticesCount = 0; // Vertex count
    unsigned _indicesCount = 0; // Index count
	Geo_Vertex* _vertices = nullptr; // formatted vertex data
	unsigned* _indices = nullptr; // index data

    Eigen::Vector3f* _posData = nullptr; // position data
	Eigen::Vector3f* _normalsData = nullptr; // normals data
    Eigen::Vector2f* _texCoordData = nullptr; // texture coordinate data
};

#define GEOMETRY_H
#endif