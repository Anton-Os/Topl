#ifndef GEO_MESH_H

#ifdef __cplusplus
#include <vector>
#include <initializer_list>
#endif

#include "Geo_Vertex.hpp"

// Shape Description

#define CIRCLE_SEGMENTS 100 // default segments for a circular object
#define DEFAULT_Z 0.0f // default depth value for objects

#define RADIUS_SIZE(radius) (radius * 0.7071f) // multiplies radius by screen units
#define MAX_INSTANCES 26 // one for each surrounding element in a recursive mesh
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

enum DRAW_Mode { DRAW_Default = 0, DRAW_Points = 1, DRAW_Lines = 2, DRAW_Triangles = 3, DRAW_Fan = 4, DRAW_Strip = 5, DRAW_Patch = 6 };

enum TFORM_Type { TFORM_Shift = 0, TFORM_Rotate = 1, TFORM_Scale = 3, TFORM_Shear = 4 };

// Mesh Object

#ifdef __cplusplus
class Geo_Mesh {
public:
	Geo_Mesh(){} // empty constructor
	Geo_Mesh(unsigned v) { _vertices.resize(v); } // vertex only
	Geo_Mesh(unsigned v, unsigned i) { _vertices.resize(v); _indices.resize(i); } // vertex and indices constructor
	Geo_Mesh(unsigned v, Geo_Vertex* vertexData){ populate(v, vertexData, 0, nullptr); }
	Geo_Mesh(unsigned v, Geo_Vertex* vertexData, unsigned i, unsigned* indexData){ populate(v, vertexData, i, indexData); }
	Geo_Mesh(vertex_cptr_t points, unsigned short pointCount);
    Geo_Mesh(const Geo_Mesh& refMesh);

	~Geo_Mesh(){ 
		if(_instanceData != nullptr){ 
			free(_instanceData); 
			_instanceData = nullptr;
		}
	}

	void modify(vTformCallback callback, Vec3f transform);
	void modify(iTformCallback callback, Vec3f transform);
	void modify(vertexTransform callback);
	void modify(originTransform callback);
	void shift(Vec3f transform) { modify(shiftTForm, transform); } // shifts position attribute
    void rotate(Vec3f transform) { modify(rotateTForm, transform); } // rotates position attribute
	void scale(Vec3f transform) { modify(scaleTForm, transform); } // scales position attribute

	void extend(const Geo_Mesh& refMesh); // extends mesh to include more geometry
	void tesselate(unsigned short tessCount); // TODO: Include non-indexed tesselation

	void setInstances(std::initializer_list<Mat4x4> matrices);

	std::string toString() const;

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

	unsigned getTessLevel() const { return _tessLevel; }
	unsigned getInstanceCount() const { return _instanceCount; }
	const Mat4x4* getInstanceData() const { return _instanceData; }

    DRAW_Mode drawMode = DRAW_Default; // by default mesh is drawn
	unsigned short drawMin = 0, drawMax = 0; // for partial mesh rendering // Check for range?
protected:
	void populate(unsigned vertexCount, Geo_Vertex* vertexData, unsigned indexCount, unsigned* indexData);

	// std::vector<Geo_Pos> _vertices;
	std::vector<Geo_Vertex> _vertices;
	std::vector<unsigned> _indices;

	unsigned _tessLevel = 0;
	unsigned _transformCount = 0;
	unsigned _instanceCount = 0;
	Mat4x4* _instanceData = nullptr;
};
#endif

#define GEO_MESH_H
#endif
