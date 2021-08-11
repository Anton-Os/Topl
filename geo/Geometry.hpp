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

#define DEFAULT_Z_VAL 0.1f
// #define DEFAULT_Z_VAL 0.0f
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
	Geo_Vertex(Eigen::Vector3f pos){ // Position data constructor
		position[X_OFFSET] = pos[X_OFFSET]; position[Y_OFFSET] = pos[Y_OFFSET]; position[Z_OFFSET] = pos[Z_OFFSET];
		texCoord[U_OFFSET] = 0.0f; texCoord[V_OFFSET] = 0.0f;
	}
	Geo_Vertex(Eigen::Vector3f pos, Eigen::Vector2f texc){ // Extended constructor
		position[X_OFFSET] = pos[X_OFFSET]; position[Y_OFFSET] = pos[Y_OFFSET]; position[Z_OFFSET] = pos[Z_OFFSET];
		texCoord[U_OFFSET] = texc[U_OFFSET]; texCoord[V_OFFSET] = texc[V_OFFSET];
	}

	float position[POSITION_COUNT];
	float texCoord[TEXCOORD_COUNT];
	// float normals[NORMALS_COUNT]; // might need to implement
};

enum VERTEX_Tag {
	VERTEX_Edge = 0,
	VERTEX_Inner = 1
	// Add more tags!
};

struct Geo_TaggedVertex : public Geo_Vertex {
	Geo_TaggedVertex(VERTEX_Tag t, Eigen::Vector3f pos)
	: Geo_Vertex(pos){ // Position data constructor
		tag = t;
	}
	Geo_TaggedVertex(VERTEX_Tag t, Eigen::Vector3f pos, Eigen::Vector2f texc)
	: Geo_Vertex(pos, texc){ // Extended constructor
		tag = t;
	}

	VERTEX_Tag tag;
};

typedef const Geo_Vertex* const geoVertex_cptr; // Safe const pointer type

struct Geo_Face {
	Eigen::Vector3f point1 = Eigen::Vector3f(1.0f, 1.0f, DEFAULT_Z_VAL); // top right
	Eigen::Vector3f point2 = Eigen::Vector3f(-1.0f, 1.0f, DEFAULT_Z_VAL); // top left
	Eigen::Vector3f point3 = Eigen::Vector3f(1.0f, -1.0f, DEFAULT_Z_VAL); // bottom right
	Eigen::Vector3f point4 = Eigen::Vector3f(-1.0f, -1.0f, DEFAULT_Z_VAL); // bottom left
};

typedef const Geo_Face* const face_cptr;

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

typedef float (*vTransformCallback)(float, double); // Callback for transforming vertex attributes given input and modifier arguments

class Geo_RenderObj {
public:
	Geo_RenderObj(unsigned v){ // Vertex only constructor
		_verticesCount = v;
		if(_verticesCount % 2 == 0) _startAngle = TOPL_PI / (_verticesCount - 1); // offset angle for each even side length

		_posData = (Eigen::Vector3f*)malloc(_verticesCount * sizeof(Eigen::Vector3f));
		_normalsData = (Eigen::Vector3f*)malloc(_verticesCount * sizeof(Eigen::Vector3f));
 		_texCoordData = (Eigen::Vector2f*)malloc(_verticesCount * sizeof(Eigen::Vector2f));
	}
    Geo_RenderObj(unsigned v, unsigned i){ // Vertex and Indices constructor
		_verticesCount = v;
		_indicesCount = i;
		if((_verticesCount - 1) % 2 == 0) _startAngle = TOPL_PI / (_verticesCount - 1); // offset angle for each even side length

		_posData = (Eigen::Vector3f*)malloc(_verticesCount * sizeof(Eigen::Vector3f));
		_normalsData = (Eigen::Vector3f*)malloc(_verticesCount * sizeof(Eigen::Vector3f));
 		_texCoordData = (Eigen::Vector2f*)malloc(_verticesCount * sizeof(Eigen::Vector2f));
		if(_indicesCount != 0) _indices = (unsigned*)malloc(_indicesCount * sizeof(unsigned));
	}
    
	~Geo_RenderObj(){ cleanup(); }
	void cleanup() {
		if (_vertices != nullptr) free(_vertices);

		if (_posData != nullptr) free(_posData);
		if (_normalsData != nullptr) free(_normalsData);
		if (_texCoordData != nullptr) free(_texCoordData);
		if (_indices != nullptr) free(_indices);
	}

	// Modify shape through vertex transform callback
	bool modify(vTransformCallback callback, double mod, AXIS_Target axis){
		if(_verticesCount == 0 || _posData == nullptr) return false; // no processing can occur

		unsigned vAttributeOffset;
		switch(axis){
			case AXIS_X: vAttributeOffset = 0; break;
			case AXIS_Y: vAttributeOffset = 1; break;
			case AXIS_Z: vAttributeOffset = 2; break;
		}
		for(unsigned v = 0; v < _verticesCount; v++) // modify the position data of each vertex
			(*(_posData + v))[vAttributeOffset] = callback((*(_posData + v))[vAttributeOffset], mod); // updates specific vertex attribute
	}

    unsigned getVerticesCount() const { return _verticesCount; } // get Vertex Count
    unsigned getIndexCount() const { return _indicesCount; } // get Index Count

	geoVertex_cptr getVertices() {
		if (_vertices == nullptr) {
			_vertices = (Geo_Vertex*)malloc(_verticesCount * sizeof(Geo_Vertex));
			for (unsigned v = 0; v < _verticesCount; v++)
				*(_vertices + v) = Geo_Vertex(*(_posData + v), *(_texCoordData + v)); // TODO: Include normals
		}
		return _vertices;
	}
    ui_cptr getIndices() const { return _indices; }
	vec3f_cptr getPosData() const { return _posData; }
	vec3f_cptr getNormalsData() const { return _normalsData; }
    vec2f_cptr getTexCoordData() const { return _texCoordData; }
protected:
	void fillRenderObject(){
		genVertices(_posData);
		genNormals(_normalsData);
		genTexCoords(_texCoordData);
        if(_indicesCount != 0) genIndices(_indices);
	}
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