#ifndef GEOMETRY_H

#include <vector>

#include "maths.h"

// Vertices

#define POSITION_COUNT 3
#define NORMALS_COUNT 3
#define TEXCOORD_COUNT 2

enum VERTEX_Attrib { VERTEX_Pos, VERTEX_Normal, VERTEX_Texcoord };

struct Geo_Vertex {
	Geo_Vertex(Vec3f pos); // Position data constructor
	Geo_Vertex(Vec3f pos, Vec2f texc); // Extended constructor

	float position[POSITION_COUNT];
	// float normals[NORMALS_COUNT]; // include if needed
	float texcoord[TEXCOORD_COUNT];
};

typedef const Geo_Vertex* const vertex_cptr_t;

// Shape Description

#define RADIAL_UNITS 0.7071f // converts specified coordinates to screen units
#define DEFAULT_CIRCLE_SEGS 1000 // default segments for a circular object
#define DEFAULT_Z 0.0f

struct NGon2D {
    float radius;
    unsigned short segments;
};

struct NGon3D {
	float radius;
	unsigned short xSegments;
	unsigned short ySegments;
};

// Transform Functions

enum AXIS_Target { AXIS_X, AXIS_Y, AXIS_Z };

typedef float (*vTformCallback)(float, double); // callback for transforming vertex attributes given input and modifier arguments

float shiftTForm(float input, double amount); // shifts point by amount
float scaleTForm(float input, double factor); // stretches point by factor

// Render Object

#define SINGLE_INSTANCE_COUNT 1

class Geo_InstancedObj {
public:
	void setInstanceCount(unsigned count) { _instanceCount = count; }
	unsigned getInstanceCount() const { return _instanceCount; }
private:
	// Add instance specific data
	unsigned _instanceCount = SINGLE_INSTANCE_COUNT; // used for instanced draw
};

class Geo_RenderObj {
public:
	Geo_RenderObj(){} // empty constructor
	Geo_RenderObj(unsigned v); // vertex only constructor
    Geo_RenderObj(unsigned v, unsigned i); // vertex and indices constructor
	Geo_RenderObj(const Geo_RenderObj& renderObj){ clone(&renderObj); }; // copy constructor
	virtual ~Geo_RenderObj(){ cleanup(); }

	void clone(const Geo_RenderObj* refObj); // copies from reference object
	// void fuse(const Geo_RenderObj* refObj); // fuses with reference object

	void modify(vTformCallback callback, double mod, AXIS_Target axis); // modify position attirbute
	void shift(double factor, AXIS_Target axis) { modify(shiftTForm, factor, axis); }; // shifts position attribute
	void scale(double factor, AXIS_Target axis) { modify(scaleTForm, factor, axis); } // scales position attribute

    unsigned getVertexCount() const { return _vertexCount; }
    unsigned getIndexCount() const { return _indexCount; }
	vertex_cptr_t getVertices() {
		genVertices();
		return _vertices;
	}
	vec3f_cptr_t getPos() const { return _pos; }
	vec3f_cptr_t getNormals() const { return _normals; }
    vec2f_cptr_t getTexCoords() const { return _texcoords; }
	ui_cptr_t getIndices() const { return _indices; }
protected:
	void init(); // called to populate internal data

	void genVertices();
    virtual void genPos(Vec3f* data) = 0;
	virtual void genNormals(Vec3f* data) = 0;
    virtual void genTexCoords(Vec2f* data) = 0;
    virtual void genIndices(unsigned* data) = 0;

	double _startAngle = 0.0; // start angle for vertex generation

    unsigned _vertexCount = 0;
	Geo_Vertex* _vertices = nullptr;
    Vec3f* _pos = nullptr;
	Vec3f* _normals = nullptr;
    Vec2f* _texcoords = nullptr;

	unsigned _indexCount = 0;
	unsigned* _indices = nullptr;
private:
	void cleanup();
};

// Implementation to replace Render Object
class Geo_Renderable {
public:
	Geo_Renderable(unsigned v); // vertex only constructor
	Geo_Renderable(unsigned v, unsigned i); // vertex and indices constructor

	void clone(const Geo_Renderable* refObj);

	void modify(vTformCallback callback, double mod, AXIS_Target axis); // modify position attirbute
	void shift(double factor, AXIS_Target axis) { modify(shiftTForm, factor, axis); }; // shifts position attribute
	void scale(double factor, AXIS_Target axis) { modify(scaleTForm, factor, axis); } // scales position attribute

	size_t getVertexCount(){ return _vertices.size(); }
	vertex_cptr_t getVertices(){ return _vertices.data(); }
	size_t getIndexCount(){ return _indices.size(); }
	ui_cptr_t getIndices(){ return _indices.data(); }
protected:
	virtual void genVertices() = 0;
	virtual void genIndices() = 0;

	std::vector<Geo_Vertex> _vertices;
	std::vector<unsigned> _indices;
};

typedef const Geo_RenderObj* const renderObj_cptr_t;

#define GEOMETRY_H
#endif