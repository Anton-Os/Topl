#ifndef GEOMETRY_H

#include <cstdlib>
#include <cmath>

#define TOPL_PI 3.141592653
#define TOPL_HALF_PI 1.57079633
#include <Eigen/Dense> // Examine documentation

#define POSITION_COUNT 3
#define TEXCOORD_COUNT 2
// #define NORMAL_COUNT 3
// #define BLENDWEIGHTS_COUNT 3

// #define DEFAULT_Z_VAL 0.5f
#define DEFAULT_Z_VAL 0.0f
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

class Geo_RenderObj {
public:
	Geo_RenderObj(){}
	// Plain vertex constructor
	Geo_RenderObj(unsigned v){
		mVertexCount = v;
	}
	// Vertex w indices constructor
    Geo_RenderObj(unsigned v, unsigned i){
		mVertexCount = v;
		mIndexCount = i;
	}
    ~Geo_RenderObj(){ cleanup(); }

	void cleanup() {
		if (mPerVertexData != nullptr) free(mPerVertexData);

		if (mPosData != nullptr) free(mPosData);
		if (mTexCoordData != nullptr) free(mTexCoordData);
		if (mIndexData != nullptr) free(mIndexData);
	}

	// Should be called in the derived class constructor body!
	void fillRenderObject(){
		mPosData = genVertices();
		mTexCoordData = genTexCoords();
        if(mIndexCount != 0) mIndexData = genIndices();
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

    vec3f_cptr getVertexData() const { return mPosData; }
    vec2f_cptr getTexCoordData() const { return mTexCoordData; }
    ui_cptr getIndexData() const { return mIndexData; }
protected:
    virtual Eigen::Vector3f* genVertices() = 0;
    virtual Eigen::Vector2f* genTexCoords() = 0;
    virtual unsigned* genIndices() = 0;

    unsigned mVertexCount = 0; // Vertex count
    unsigned mIndexCount = 0; // Index count

	Geo_PerVertexData* mPerVertexData = nullptr; // Formatted per vertex data
    Eigen::Vector3f* mPosData = nullptr; // Vertex data
    Eigen::Vector2f* mTexCoordData = nullptr; // Texture coordinate data
	unsigned* mIndexData = nullptr; // Index data
};

#define GEOMETRY_H
#endif