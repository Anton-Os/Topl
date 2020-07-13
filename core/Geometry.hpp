#ifndef GEOMETRY_H

#include <cstdlib>
#include <cmath>

#include <vector>
#include <map>
#include <string> // For the scenegraph

#define TOPL_PI 3.141592653589793238462643383279502884197169399375105820974944592307816406
#include <Eigen/Dense> // Examine documentation

// Simple Primitive Types

struct Rect {
    float width;
    float height;
};

struct Circle {
    float radius;
    unsigned segments;
};

// Renderable primitive types

// Typedefs for "safe" types
typedef const Eigen::Vector3f* const vec3f_cptr;
typedef const Eigen::Vector2f* const vec2f_cptr;
typedef const unsigned* const ui_cptr;

struct Geo_PerVertexData {
	Geo_PerVertexData() : position(nullptr), texcoord(nullptr) {} // Empty constructor

	Geo_PerVertexData(vec3f_cptr p, vec2f_cptr t) : position(p), texcoord(t) {}

	Geo_PerVertexData Geo_PerVertexData::operator=(const Geo_PerVertexData& data) {
		return Geo_PerVertexData( data.position, data.texcoord );
	}
	vec3f_cptr position;
	vec2f_cptr texcoord;
};

typedef const Geo_PerVertexData* const perVertex_cptr;

class Geo_RenderObj {
public:
    Geo_RenderObj(){}
    ~Geo_RenderObj(){ cleanup(); }

	void cleanup() {
		if (mVData != nullptr) free(mVData);
		if (mTData != nullptr) free(mTData);
		if (mIData != nullptr) free(mIData);
	}

    unsigned getVCount() const { return mVCount; }
    unsigned getICount() const { return mICount; }
	perVertex_cptr getPerVertexData() {
		if (mPerVertexData.size() == 0) {
			// mPerVertexData.resize(mVCount);
			/* for (std::vector<Geo_PerVertexData>::iterator currentElem = mPerVertexData.begin(); currentElem < mPerVertexData.end(); currentElem++) {
				*(currentElem) = { mVData + vOffset, mTData + vOffset };
				vOffset++;
			} */

			for (unsigned vOffset = 0; vOffset < mVCount; vOffset++)
				mPerVertexData.push_back(Geo_PerVertexData(mVData + vOffset, mTData + vOffset));
		}
		return mPerVertexData.data();
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

	std::vector<Geo_PerVertexData> mPerVertexData;

    Eigen::Vector3f* mVData = nullptr; // Vertex data
    Eigen::Vector2f* mTData = nullptr; // Texture coordinate data
	unsigned* mIData = nullptr; // Index data
};

// Override the virtual functions above
class Geo_Rect2D : public Geo_RenderObj {
public:
	Geo_Rect2D() : Geo_RenderObj() {}
    Geo_Rect2D(float width, float height) : Geo_RenderObj() {
        mVCount = 4; // Rectangle has 4 vertices
        mICount = 6; // Rectangle has 6 indices
        mRect.width = width;
        mRect.height = height;

        mVData = genVertices();
        mIData = genIndices();
		mTData = genTexCoords();
    }

	float getWidth() const { return mRect.width; }
	float getHeight() const { return mRect.height; }
private:
    Eigen::Vector3f* genVertices() override;
	Eigen::Vector2f* genTexCoords() override;
    unsigned* genIndices() override;
    Rect mRect;
};

class Geo_Sphere2D : Geo_RenderObj {
public:
    Geo_Sphere2D(float radius, unsigned segments) : Geo_RenderObj() {
        mVCount = segments + 1; // Number of segments + center point
        mICount = segments * 3; // Rectangle has 6 indices
        mCircle.radius = radius;
        mCircle.segments = segments;

        mVData = genVertices();
        mIData = genIndices();
    }
private:
    Eigen::Vector3f* genVertices() override;
	Eigen::Vector2f* genTexCoords() override;
    unsigned* genIndices() override;
    Circle mCircle;
};

#define GEOMETRY_H
#endif