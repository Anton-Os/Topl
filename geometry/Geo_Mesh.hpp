#ifndef GEO_MESH_H

#include <vector>
#include <initializer_list>

#include "Geo_Vertex.hpp"

// Shape Description

#define CIRCLE_SEGMENTS 100 // default segments for a circular object
#define DEFAULT_Z 0.0f // default depth value for objects

#define RADIUS_SIZE(radius) (radius * 0.7071f) // multiplies radius by screen units
#define ANGLE_OFFSET(segments) ((3.141592653 * 2) / segments)
#define ANGLE_START(segments) ((segments % 2 == 0) ? (3.141592653 / segments) : (0.0f))

struct Shape2D {
	float radius;
    unsigned short segments;
};

struct Shape3D {
	float radius;
	unsigned short xSegs, ySegs;
};

enum DRAW_Mode { DRAW_Points, DRAW_Lines, DRAW_Triangles, DRAW_Fan, DRAW_Strip, DRAW_Patch };

// Mesh Object

class Geo_Mesh {
public:
	Geo_Mesh(unsigned v) { _vertices.resize(v); } // vertex only
	Geo_Mesh(unsigned v, unsigned i) { _vertices.resize(v); _indices.resize(i); } // vertex and indices constructor

	Geo_Mesh(Vec3f* points, unsigned short pointCount){ // point set constructor
		_vertices.resize(pointCount); 
		_indices.resize(pointCount);

		for(auto p = 0; p < pointCount; p++){
			_vertices[p] = *(points + p);
			_vertices[p].texcoord = _vertices[p].position;
			_indices[p] = p; // for testing
		}
	}
	
    Geo_Mesh(const Geo_Mesh& refMesh){
		for(unsigned short v = 0; v < refMesh.getVertexCount(); v++) _vertices.push_back(*(refMesh.getVertices() + v));
		for(unsigned short i = 0; i < refMesh.getIndexCount(); i++) _indices.push_back(*(refMesh.getIndices() + i));
	}

	void modify(vTformCallback callback, Vec3f transform) { // modify position attirbute
		for (std::vector<Geo_Vertex>::iterator v = _vertices.begin(); v != _vertices.end(); v++)
			*(&v->position) = callback(v->position, transform);
	}
	void shift(Vec3f transform) { modify(shiftTForm, transform); } // shifts position attribute
    void rotate(Vec3f transform) { modify(rotateTForm, transform); } // rotates position attribute
	void scale(Vec3f transform) { modify(scaleTForm, transform); } // scales position attribute

    Vec3f getOrigin() const {
        Vec3f origin = VEC_3F_ZERO;
        for(unsigned v = 0; v < getVertexCount(); v++) origin = _vertices[v].position + origin;
        return origin * Vec3f({ 1.0F / getVertexCount(), 1.0F / getVertexCount(), 1.0F / getVertexCount() });
    }
	// float[6] getBounds(){ return { 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F }; } // TODO: Compute bounds
	size_t getVertexCount() const { return _vertices.size(); }
	vertex_cptr_t getVertices() const { return _vertices.data(); }
	size_t getIndexCount() const { return _indices.size(); }
	ui_cptr_t getIndices() const { return _indices.data(); }

	bool instanceCount = 0;
    bool isTesselated = false;
    DRAW_Mode drawMode = DRAW_Triangles; // Include draw mode as part of mesh?
protected:
	virtual void genVertices(){}
	virtual void genIndices(){}

	std::vector<Geo_Vertex> _vertices;
	std::vector<unsigned> _indices;
};

#define GEO_MESH_H
#endif
