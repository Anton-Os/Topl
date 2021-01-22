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

#define X_OFFSET 0
#define Y_OFFSET 1
#define Z_OFFSET 2
#define U_OFFSET 0
#define V_OFFSET 1

// Typedefs for const safe types
typedef const Eigen::Vector3f* const vec3f_cptr;
typedef const Eigen::Vector2f* const vec2f_cptr;
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
    ~Geo_RenderObj(){ cleanup(); }

	void cleanup() {
		if (mPerVertexData != nullptr) free(mPerVertexData);

		if (mVData != nullptr) free(mVData);
		if (mTData != nullptr) free(mTData);
		if (mIData != nullptr) free(mIData);
	}

    unsigned getVCount() const { return mVCount; }
    unsigned getICount() const { return mICount; }
	perVertex_cptr getPerVertexData() {

		if (mPerVertexData == nullptr) {
			mPerVertexData = (Geo_PerVertexData*)malloc(mVCount * sizeof(Geo_PerVertexData));
			for (unsigned v = 0; v < mVCount; v++)
				*(mPerVertexData + v) = Geo_PerVertexData(*(mVData + v), *(mTData + v));
		}
		return mPerVertexData;
	}

    vec3f_cptr getVData() const { return mVData; }
    vec2f_cptr getTData() const { return mTData; }
    ui_cptr getIData() const { return mIData; }
protected:
    virtual Eigen::Vector3f* genVertices() = 0;
    virtual Eigen::Vector2f* genTexCoords() = 0;
    virtual unsigned* genIndices() = 0;

    const unsigned short mPerVertex = 5; // Elements per vertex, should be configurable later
    unsigned mVCount;
    unsigned mICount;

	//std::vector<Geo_PerVertexData> mPerVertexData;
	Geo_PerVertexData* mPerVertexData = nullptr;

    Eigen::Vector3f* mVData = nullptr; // Vertex data
    Eigen::Vector2f* mTData = nullptr; // Texture coordinate data
	unsigned* mIData = nullptr; // Index data
};

// #include "primitives/Geo_Rect2D.hpp"
// #include "primitives/Geo_Sphere2D.hpp"

#define GEOMETRY_H
#endif