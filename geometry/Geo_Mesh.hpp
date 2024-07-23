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

struct Shape {  // TODO: Change to Shape2D
	float radius;
    unsigned short segments;
};

struct Volume { // TODO: Change to Shape3D
	float radius;
	unsigned short xSegs, ySegs;
};

// Mesh Object

#include <iostream>
#include <string>

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
	
	Geo_Mesh(Geo_Mesh& refMesh){
		if(refMesh.getVertexCount() == 0) std::cerr << "Mesh with no vertices detected" << std::endl;
		// else std::cout << "Copy constructor invoked" << std::endl;
		for(unsigned short v = 0; v < refMesh.getVertexCount(); v++) _vertices.push_back(*(refMesh.getVertices() + v));
		for(unsigned short i = 0; i < refMesh.getIndexCount(); i++) _indices.push_back(*(refMesh.getIndices() + i));
	}

	void modify(vTformCallback callback, Vec3f transform) { // modify position attirbute
		for (std::vector<Geo_Vertex>::iterator v = _vertices.begin(); v != _vertices.end(); v++)
			*(&v->position) = callback(v->position, transform);
	}
	void shift(Vec3f transform) { modify(shiftTForm, transform); } // shifts position attribute
	// void rotate(Vec3f angles) { modify(rotateTForm, transform); } // rotates position attribute
	void scale(Vec3f transform) { modify(scaleTForm, transform); } // scales position attribute

	Vec3f getOrigin(){ return Vec3f({ 0.0F, 0.0F, 0.0F }); } // TODO: Compute origin
	// float[6] getBounds(){ return { 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F }; } // TODO: Compute bounds
	size_t getVertexCount() const { return _vertices.size(); }
	vertex_cptr_t getVertices() const { return _vertices.data(); }
	size_t getIndexCount() const { return _indices.size(); }
	ui_cptr_t getIndices() const { return _indices.data(); }
protected:
	virtual void genVertices(){}
	virtual void genIndices(){}

	Vec3f getTexCoord(unsigned v, float z) { // default texture coordinate generator
		float x = ((v - 1) % 4 == 0 || (v - 1) % 4 == 1)? 1.0f : 0.0f;
		float y = ((v - 1) % 4 == 1 || (v - 1) % 4 == 2) ? 1.0f : 0.0f;
		return Vec3f({ x, y, z });
	}

	std::vector<Geo_Vertex> _vertices;
	std::vector<unsigned> _indices;
};

#define GEO_MESH_H
#endif