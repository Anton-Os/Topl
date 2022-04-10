#ifndef GEOMETRY_H

#include "Maths.h"

#define POSITION_COUNT 3
#define NORMALS_COUNT 3
#define TEXCOORD_COUNT 2

#define RADIAL_UNITS 0.7071f // converts specified coordinates to screen units
#define DEFAULT_CIRCLE_SEGS 1000 // default segments for a circular object
#define DEFAULT_Z 0.0f

enum VERTEX_Attrib { VERTEX_Pos, VERTEX_Normal, VERTEX_Texcoord };

enum VERTEX_Shift {
	VERTEX_Forward, // shifts data 1 element forward
	VERTEX_Reverse, // shifts data 1 element in reverse
	VERTEX_Flip // flips all data internally
};

struct Geo_Vertex {
	Geo_Vertex(Vec3f pos); // Position data constructor
	Geo_Vertex(Vec3f pos, Vec2f texc); // Extended constructor

	float position[POSITION_COUNT];
	float texcoord[TEXCOORD_COUNT];
	// float normals[NORMALS_COUNT]; // might need to implement
};

typedef const Geo_Vertex* const vertex_cptr_t;

struct NGon2D {
    float radius;
    unsigned short segments;
};

struct NGon3D {
	float radius;
	unsigned short xSegments;
	unsigned short ySegments;
};

enum AXIS_Target{ AXIS_X, AXIS_Y, AXIS_Z };

typedef float (*vTformCallback)(float, double); // callback for transforming vertex attributes given input and modifier arguments

class Geo_RenderObj {
public:
	Geo_RenderObj(){} // empty constructor
	Geo_RenderObj(unsigned v); // vertex only constructor
    Geo_RenderObj(unsigned v, unsigned i); // vertex and indices constructor
	Geo_RenderObj(const Geo_RenderObj& renderObj){ clone(&renderObj); }; // copy constructor
    
	virtual ~Geo_RenderObj(){ cleanup(); }
	void clone(const Geo_RenderObj* refObj); // copies render object
	void modify(vTformCallback callback, double mod, AXIS_Target axis); // modifies position attirbute
	void reorder(enum VERTEX_Attrib attrib, VERTEX_Shift shift); // shifts vertex attribute data
	// void fuse(const Geo_RenderObj* refObj); // fuses target object with reference object

    unsigned getVerticesCount() const { return _verticesCount; } // get vertex Count
    unsigned getIndexCount() const { return _indicesCount; } // get index Count

	vertex_cptr_t getVertices() {
		genVertices(); // vertices are generated here since vertex format may be subject to change
		return _vertices;
	}
    ui_cptr_t getIndices() const { return _indices; }
	vec3f_cptr_t getPosData() const { return _posData; }
	vec3f_cptr_t getNormalsData() const { return _normalsData; }
    vec2f_cptr_t getTexCoordData() const { return _texcoordData; }
protected:
	void fillRenderObj(); // called by derived class to populate vertex attributes
	void genVertices(); // internally generates data based on vertex format
    virtual void genPos(Vec3f* data) = 0;
	virtual void genNormals(Vec3f* data) = 0;
    virtual void genTexCoords(Vec2f* data) = 0;
    virtual void genIndices(unsigned* data) = 0;

	double _startAngle = 0.0; // starting angle for vertex generation

    unsigned _verticesCount = 0; // vertex count
	Geo_Vertex* _vertices = nullptr; // formatted vertex data
    Vec3f* _posData = nullptr; // position data
	Vec3f* _normalsData = nullptr; // normals data
    Vec2f* _texcoordData = nullptr; // texture coordinate data

	unsigned _indicesCount = 0; // index count
	unsigned* _indices = nullptr; // index data
private:
	void cleanup(); // used to erase all internal data (destroy)
};

typedef const Geo_RenderObj* const renderObj_cptr_t;

#define GEOMETRY_H
#endif