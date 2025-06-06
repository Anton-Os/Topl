#ifndef GEO_MESH_H

#include <vector>
#include <initializer_list>

#include "Geo_Vertex.hpp"

// Shape Description

#define CIRCLE_SEGMENTS 100 // default segments for a circular object
#define DEFAULT_Z 0.0f // default depth value for objects

#define RADIUS_SIZE(radius) (radius * 0.7071f) // multiplies radius by screen units
#define MAX_INSTANCES 32
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

class Geo_Mesh {
public:
	Geo_Mesh(){} // empty constructor
	Geo_Mesh(unsigned v) { _vertices.resize(v); } // vertex only
	Geo_Mesh(unsigned v, unsigned i) { _vertices.resize(v); _indices.resize(i); } // vertex and indices constructor

	~Geo_Mesh(){ if(_instanceData != nullptr) free(_instanceData); }

	Geo_Mesh(vertex_cptr_t points, unsigned short pointCount){ // point set constructor
		_vertices.resize(pointCount); 
		_indices.resize(pointCount);

		for(auto p = 0; p < pointCount; p++){
			_vertices[p] = *(points + p);
			_indices[p] = p; // choose indexing scheme?
		}
	}
	
    Geo_Mesh(const Geo_Mesh& refMesh){
		for(unsigned short v = 0; v < refMesh.getVertexCount(); v++) _vertices.push_back(*(refMesh.getVertices() + v));
		for(unsigned short i = 0; i < refMesh.getIndexCount(); i++) _indices.push_back(*(refMesh.getIndices() + i));
		// TODO: Copy instance and other data?
	}

	void modify(vTformCallback callback, Vec3f transform) { // modify position attirbute
		for (std::vector<Geo_Vertex>::iterator v = _vertices.begin(); v != _vertices.end(); v++)
			*(&v->position) = callback(v->position, transform);
		_transformCount++;
	}
	void modify(iTformCallback callback, Vec3f transform){
		for (std::vector<unsigned>::iterator i = _indices.begin(); i != _indices.end(); i++)
			_vertices[*i] = callback(*i, _vertices[*i].position, transform);
		_transformCount++;
	}
	void modify(vertexTransform callback){
		for (unsigned v = 0; v < _vertices.size(); v++)
			_vertices[v] = callback(_vertices[v], v / 3, _transformCount);
		_transformCount++;
	}
	void modify(originTransform callback){
		bool isIndexed = getIndexCount() > 0;
		for (unsigned m = 0; m < ((isIndexed)? _indices.size() : _vertices.size()) / 3; m++){
			if(m + 2 > _indices.size() && _indices.size() > 0) break;
			unsigned idx1 = (isIndexed)? _indices[(m * 3) + 0] : (m * 3) + 0;
			unsigned idx2 = (isIndexed)? _indices[(m * 3) + 1] : (m * 3) + 1;
			unsigned idx3 = (isIndexed)? _indices[(m * 3) + 2] : (m * 3) + 2;
			Geo_Vertex midpoint = Geo_Vertex((_vertices[idx1].position + _vertices[idx2].position + _vertices[idx3].position) * (1.0f / 3.0f));
			// Geo_Vertex midpoint = getMidpoint({ _vertices[idx1], _vertices[idx2], _vertices[idx3] }); // TODO: Get this working!
			_vertices[idx1] = callback(_vertices[idx1], midpoint, m, _transformCount);
			_vertices[idx2] = callback(_vertices[idx2], midpoint, m, _transformCount);
			_vertices[idx3] = callback(_vertices[idx3], midpoint, m, _transformCount);
		}
		_transformCount++;
	}
	void shift(Vec3f transform) { modify(shiftTForm, transform); } // shifts position attribute
    void rotate(Vec3f transform) { modify(rotateTForm, transform); } // rotates position attribute
	void scale(Vec3f transform) { modify(scaleTForm, transform); } // scales position attribute

	void extend(const Geo_Mesh& refMesh){
		unsigned svCount = getVertexCount();
		
		for(unsigned v = 0; v < refMesh.getVertexCount(); v++)
			_vertices.push_back(*(refMesh.getVertices() + v));

		for(unsigned i = 0; i < refMesh.getIndexCount(); i++)
			_indices.push_back(*(refMesh.getIndices() + i) + svCount);
	}

	void tesselate(unsigned short tessCount){ // TODO: Include Tesselation for Non-Indexed Mesh
		if(tessCount == 0) return;
		unsigned short svCount, siCount;

		for(unsigned l = 0; l < tessCount; l++){
			svCount = getVertexCount(); // start vertex count
			siCount = getIndexCount(); // start index count

			if(siCount > 0){
				for(unsigned i = 0; i < siCount; i += 3) // determining tesselated vertices
				  _vertices.push_back(Geo_Vertex(Vec3f((_vertices[_indices[i]].position + _vertices[_indices[i + 1]].position + _vertices[_indices[i + 2]].position) * (1.0 / 3.0))));
				  // _vertices.push_back(getMidpoint({ _vertices[_indices[i]], _vertices[_indices[i + 1]], _vertices[_indices[i + 2]] })); // TODO: Get this working!

				for(unsigned i = 0; i < siCount; i += 3){
					unsigned v = (svCount + (i / 3));
					_indices.push_back(_indices[i]);
					_indices.push_back(_indices[i + 1]);
					_indices.push_back(v); // first triangle
					_indices.push_back(_indices[i + 2]);
					_indices.push_back(_indices[i + 1]);
					_indices.push_back(v); // second triangle
					_indices.push_back(_indices[i]);
					_indices.push_back(_indices[i + 2]);
					_indices.push_back(v); // third triangle
				}
				for (unsigned i = 0; i < siCount; i++) _indices.erase(_indices.begin());
			} else {
				for(unsigned v = 0; v < svCount; v += 3){
					Geo_Vertex midpoint = Geo_Vertex(Vec3f((_vertices[v + 0].position + _vertices[v + 1].position + _vertices[v + 2].position) * (1.0 / 3.0)));
					_vertices.push_back(midpoint); _vertices.push_back(_vertices[v]); _vertices.push_back(_vertices[v + 1]); // 1st triangle
                    _vertices.push_back(midpoint); _vertices.push_back(_vertices[v]); _vertices.push_back(_vertices[v + 2]); // 2nd triangle
                    _vertices.push_back(midpoint); _vertices.push_back(_vertices[v + 1]); _vertices.push_back(_vertices[v + 2]); // 3rd triangle
				}
				for (unsigned v = 0; v < svCount; v++) _vertices.erase(_vertices.begin());
			}

			drawMax = (siCount > 0)? _indices.size() : _vertices.size();
		}

		_tessLevel += tessCount; // for testing
	}

	void setInstances(std::initializer_list<Mat4x4> matrices){
		_instanceCount = matrices.size();
		if(_instanceData != nullptr) free(_instanceData);
		_instanceData = (Mat4x4*)malloc(matrices.size() * sizeof(Mat4x4));
		
		unsigned instanceOffset = 0;
		for(auto m = matrices.begin(); m != matrices.end() && instanceOffset < MAX_INSTANCES; m++){
			*(_instanceData + instanceOffset) = *m;
			instanceOffset++;
		}
	}

	std::string toString() const { // creates a printed version of a mesh
		std:: string meshStr = "Vertices: {";
		for(unsigned v = 0; v < getVertexCount(); v++) meshStr += _vertices[v].position.toString() + ", ";
		meshStr += (getIndexCount() > 0)? "}\nIndices: {" : "}";
		if(getIndexCount() > 0){
			for(unsigned i = 0; i < getIndexCount(); i++) meshStr += std::to_string(i) + ", ";
			meshStr += "}";
		}
		return meshStr;
	}
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
	// std::vector<Geo_Pos> _vertices;
	std::vector<Geo_Vertex> _vertices;
	std::vector<unsigned> _indices;

	unsigned _tessLevel = 0;
	unsigned _transformCount = 0;
	unsigned _instanceCount = 0;
	Mat4x4* _instanceData = nullptr;
};

#define GEO_MESH_H
#endif
