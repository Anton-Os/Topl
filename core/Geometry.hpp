#ifndef GEOMETRY_H

#include <cstdlib>
#include <cmath>

#include <vector>
#include <map>
#include <string> // For the scenegraph

#define TOPL_PI 3.141592653
#define TOPL_HALF_PI 1.57079633
#include <Eigen/Dense> // Examine documentation

// Typedefs for "safe" types
typedef const Eigen::Vector3f* const vec3f_cptr;
typedef const Eigen::Vector2f* const vec2f_cptr;
typedef const unsigned* const ui_cptr;

struct Geo_PerVertexData { // TODO: Fix this class
	Geo_PerVertexData(){} // Empty constructor

	Geo_PerVertexData(Eigen::Vector3f p){ // Position Parameter
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
	}
	Geo_PerVertexData(Eigen::Vector3f p, Eigen::Vector2f t){ // Position + Texture Coords + Other Params
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
		texCoord[0] = t[0]; texCoord[1] = t[1];
	}

	float position[3]; // For xyz
	float texCoord[2]; // For uv texture coordinates */
};

typedef const Geo_PerVertexData* const perVertex_cptr;

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

#include "primitives/Geo_Rect2D.hpp"

#include "primitives/Geo_Sphere2D.hpp" // Definition of Sphere2D class

#define GEOMETRY_H
#endif