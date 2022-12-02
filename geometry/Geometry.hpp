#ifndef GEOMETRY_H

#include <vector>

#include "maths.h"

// Vertices

enum VERTEX_Attrib { VERTEX_Pos, VERTEX_Normal, VERTEX_Texcoord };

struct Geo_Vertex {
	Geo_Vertex() {} // empty constructor
	Geo_Vertex(Vec3f p) { // position constructor
		position = p; texcoord = Vec3f({ 0.0, 0.0, 0.0 });
	}
	Geo_Vertex(Vec3f p, Vec3f t) {// full constructor
		position = p; texcoord = t;
	}

	Vec3f position;
	// Vec3f normal;
	Vec3f texcoord;
};

typedef const Geo_Vertex* const vertex_cptr_t;

// Shape Description

#define SCREEN_UNITS 0.7071f // converts specified coordinates to screen units
#define CIRCLE_SEGMENTS 1000 // default segments for a circular object
#define DEFAULT_Z 0.0f // default depth value for objects
#define Z_INCREMENT 0.00001f // increment to depth prevents geometry obstruction

struct Shape2D {
	float getSize() const { return radius * SCREEN_UNITS; } // radius converted to screen units
	float getAngle() const { return (MATH_PI * 2) / segments; } // angle increment between vertices
	float getInitAngle() const { return (segments % 2 == 0) ? MATH_PI / segments : 0.0; } // starting angle for vertices
    
	float radius;
    unsigned short segments;
};

struct Shape3D {
	float getSize() const { return radius * SCREEN_UNITS; } // radius converted to screen units
	float getAngleX() const { return (MATH_PI * 2) / xSegments; } // angle increment on x axis
	float getAngleY() const { return (MATH_PI * 2) / ySegments; } // angle increment on y axis

	float radius;
	unsigned short xSegments;
	unsigned short ySegments;
};

// Transform Functions

enum AXIS_Target { 
	AXIS_X = 0, 
	AXIS_Y = 1, 
	AXIS_Z = 2 
};

typedef float (*vTformCallback)(float, double); // callback for transforming vertex attributes given input and modifier arguments

static float shiftTForm(float input, double amount) { return input + amount; } // shifts point by amount
static float scaleTForm(float input, double factor) { return input * factor; } // scales point by factor

// Renderable Object

class Geo_Renderable {
public:
	Geo_Renderable(unsigned v) { // vertex only constructor
		_vertices.resize(v);
	}
	
	Geo_Renderable(unsigned v, unsigned i) { // vertex and indices constructor
		_vertices.resize(v);
		_indices.resize(i);
	}

	void clone(const Geo_Renderable* refObj);

	void modify(vTformCallback callback, double mod, AXIS_Target axis) { // modify position attirbute
		for (std::vector<Geo_Vertex>::iterator v = _vertices.begin(); v != _vertices.end(); v++)
			v->position[axis] = callback(v->position[axis], mod);
	}
	void shift(double factor, AXIS_Target axis) { modify(shiftTForm, factor, axis); }; // shifts position attribute
	void scale(double factor, AXIS_Target axis) { modify(scaleTForm, factor, axis); } // scales position attribute

	size_t getVertexCount() const { return _vertices.size(); }
	vertex_cptr_t getVertices(){ return _vertices.data(); }
	size_t getIndexCount() const { return _indices.size(); }
	ui_cptr_t getIndices(){ return _indices.data(); }
protected:
	virtual void genVertices() = 0;
	virtual void genIndices() = 0;

	static Vec3f getTexCoord(unsigned v, float z) { // get default texcoord from vertex number
		float x = ((v - 1) % 4 == 0 || (v - 1) % 4 == 1)? 1.0f : 0.0f;
		float y = ((v - 1) % 4 == 1 || (v - 1) % 4 == 2) ? 1.0f : 0.0f;
		return Vec3f({ x, y, z });
	}

	std::pair<bool, bool> _renderStatus = std::make_pair(false, false); // checks for vertex generation
	std::vector<Geo_Vertex> _vertices;
	std::vector<unsigned> _indices;
};

#define GEOMETRY_H
#endif