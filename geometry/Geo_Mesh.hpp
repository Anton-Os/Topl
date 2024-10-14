#ifndef GEO_MESH_H

#include <vector>

#include "Geo_Vertex.hpp"

// Shape Description

#define CIRCLE_SEGMENTS 100 // default segments for a circular object
#define DEFAULT_Z 0.0f // default depth value for objects

#define RADIUS_SIZE(radius) (radius * 0.7071f) // multiplies radius by screen units
#define ANGLE_OFFSET(segments) ((3.141592653 * 2) / segments)
#define ANGLE_START(segments) ((segments % 2 == 0) ? (3.141592653 / segments) : (0.0f))

struct Surface {
	float radius;
    unsigned short segments;
};

struct Volume {
	float radius;
	unsigned short xSegs, ySegs;
};

// Mesh Object

class Geo_Mesh {
public:
	Geo_Mesh(unsigned v) { _vertices.resize(v); } // vertex only
	
	Geo_Mesh(unsigned v, unsigned i) { // vertex and indices constructor
		_vertices.resize(v);
		_indices.resize(i);
	}

	void modify(vTformCallback callback, Vec3f transform) { // modify position attirbute
		for (std::vector<Geo_Vertex>::iterator v = _vertices.begin(); v != _vertices.end(); v++)
			*(&v->position) = callback(v->position, transform);
	}
	void shift(Vec3f transform) { modify(shiftTForm, transform); } // shifts position attribute
	// void rotate(Vec3f angles) { modify(rotateTForm, transform); } // rotates position attribute
	void scale(Vec3f transform) { modify(scaleTForm, transform); } // scales position attribute

	size_t getVertexCount() const { return _vertices.size(); }
	vertex_cptr_t getVertices() const { return _vertices.data(); }
	size_t getIndexCount() const { return _indices.size(); }
	ui_cptr_t getIndices() const { return _indices.data(); }

    bool isTesselated = false;
protected:
	virtual void genVertices() = 0;
	virtual void genIndices() = 0;

	static Vec3f getTexCoord(unsigned v, float z) { // default texture coordinate generator
		float x = ((v - 1) % 4 == 0 || (v - 1) % 4 == 1)? 1.0f : 0.0f;
		float y = ((v - 1) % 4 == 1 || (v - 1) % 4 == 2) ? 1.0f : 0.0f;
		return Vec3f({ x, y, z });
	}

	std::vector<Geo_Vertex> _vertices;
	std::vector<unsigned> _indices;
};

#define GEO_MESH_H
#endif
