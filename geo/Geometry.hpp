#ifndef GEOMETRY_H

#include <cstdlib>
#include <cmath>

#define DEFAULT_CIRCLE_SEGS 1001 // this is a chiliagon + 1 for odd sides
#define TOPL_PI 3.141592653
#define TOPL_HALF_PI 1.57079633
#include <Eigen/Dense> // Examine documentation

#define POSITION_COUNT 3
#define TEXCOORD_COUNT 2
// #define NORMAL_COUNT 3
// #define BLENDWEIGHTS_COUNT 3

#define DEFAULT_Z_VAL 0.5f
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

enum AXIS_Target{
    AXIS_X,
    AXIS_Y,
    AXIS_Z
};

typedef float (*vertexFuncCallback1)(float, double); // Callback for transforming vertex attributes given input and modifier 

struct Geo_PerVertexData { // TODO: Fix this class
	Geo_PerVertexData(){} // Empty constructor

	Geo_PerVertexData(Eigen::Vector3f p){ // Position data constructor
		position[X_OFFSET] = p[X_OFFSET]; position[Y_OFFSET] = p[Y_OFFSET]; position[Z_OFFSET] = p[Z_OFFSET];
		texCoord[U_OFFSET] = 0.0f; texCoord[V_OFFSET] = 0.0f;
	}
	Geo_PerVertexData(Eigen::Vector3f p, Eigen::Vector2f t){ // Extended constructor
		position[X_OFFSET] = p[X_OFFSET]; position[Y_OFFSET] = p[Y_OFFSET]; position[Z_OFFSET] = p[Z_OFFSET];
		texCoord[U_OFFSET] = t[U_OFFSET]; texCoord[V_OFFSET] = t[V_OFFSET];
	}

	float position[POSITION_COUNT];
	float texCoord[TEXCOORD_COUNT];
	// float normal[NORMAL_COUNT]; // Implement
	// float blendWeights[BLENDWEIGHTS_COUNT]; // Implement
};

typedef const Geo_PerVertexData* const perVertex_cptr; // Safe const pointer type

struct NGon2D {
    float radius;
    unsigned short segments;
};

struct NGon3D {
	float radius;
	unsigned short xSegments;
	unsigned short ySegments;
};

class Geo_RenderObj {
public:
	Geo_RenderObj(){}
	// Vertex only constructor
	Geo_RenderObj(unsigned v){
		mVertexCount = v;
		if(mVertexCount % 2 == 0) mStartAngle = TOPL_PI / (mVertexCount - 1); // offset angle for each even side length

		mPosData = (Eigen::Vector3f*)malloc(mVertexCount * sizeof(Eigen::Vector3f));
 		mTexCoordData = (Eigen::Vector2f*)malloc(mVertexCount * sizeof(Eigen::Vector2f));
	}
	// Vertex and Indices constructor
    Geo_RenderObj(unsigned v, unsigned i){
		mVertexCount = v;
		mIndexCount = i;
		if((mVertexCount - 1) % 2 == 0) mStartAngle = TOPL_PI / (mVertexCount - 1); // offset angle for each even side length

		mPosData = (Eigen::Vector3f*)malloc(mVertexCount * sizeof(Eigen::Vector3f));
 		mTexCoordData = (Eigen::Vector2f*)malloc(mVertexCount * sizeof(Eigen::Vector2f));
		if(mIndexCount != 0) mIndexData = (unsigned*)malloc(mIndexCount * sizeof(unsigned));
	}
    
	~Geo_RenderObj(){ cleanup(); }
	void cleanup() {
		if (mPerVertexData != nullptr) free(mPerVertexData);

		if (mPosData != nullptr) free(mPosData);
		if (mTexCoordData != nullptr) free(mTexCoordData);
		if (mIndexData != nullptr) free(mIndexData);
	}

	// Modify shape through vertex transform callback
	bool modShapeFunc(vertexFuncCallback1 callback, double mod, AXIS_Target axis){
		if(mVertexCount == 0 || mPosData == nullptr) return false; // no processing can occur

		unsigned vAttributeOffset;
		switch(axis){
			case AXIS_X: vAttributeOffset = 0; break;
			case AXIS_Y: vAttributeOffset = 1; break;
			case AXIS_Z: vAttributeOffset = 2; break;
		}
		for(unsigned v = 0; v < mVertexCount; v++)
			(*(mPosData + v))[vAttributeOffset] = callback((*(mPosData + v))[vAttributeOffset], mod); // updates specific vertex attribute
	}

	perVertex_cptr getPerVertexData() {
		if (mPerVertexData == nullptr) {
			mPerVertexData = (Geo_PerVertexData*)malloc(mVertexCount * sizeof(Geo_PerVertexData));
			for (unsigned v = 0; v < mVertexCount; v++)
				*(mPerVertexData + v) = Geo_PerVertexData(*(mPosData + v), *(mTexCoordData + v));
		}
		return mPerVertexData;
	}
    unsigned getVertexCount() const { return mVertexCount; } // Get Vertex Count
    unsigned getIndexCount() const { return mIndexCount; } // Get Index Count

    vec3f_cptr getPosData() const { return mPosData; }
    vec2f_cptr getTexCoordData() const { return mTexCoordData; }
    ui_cptr getIndexData() const { return mIndexData; }
protected:
	void fillRenderObject(){
		genVertices(mPosData);
		genTexCoords(mTexCoordData);
        if(mIndexCount != 0) genIndices(mIndexData);
	}
    virtual void genVertices(Eigen::Vector3f* data) = 0;
    virtual void genTexCoords(Eigen::Vector2f* data) = 0;
    virtual void genIndices(unsigned* data) = 0;

    unsigned mVertexCount = 0; // Vertex count
    unsigned mIndexCount = 0; // Index count
	double mStartAngle = 0.0; // Start angle for Vertex generation

	Geo_PerVertexData* mPerVertexData = nullptr; // Formatted per vertex data
    Eigen::Vector3f* mPosData = nullptr; // Vertex data
	Eigen::Vector3f* mNormalsData = nullptr; // Normals data
    Eigen::Vector2f* mTexCoordData = nullptr; // Texture coordinate data
	unsigned* mIndexData = nullptr; // Index data
};

#define GEOMETRY_H
#endif