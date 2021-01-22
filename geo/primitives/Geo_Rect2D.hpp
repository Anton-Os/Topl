#include "Geometry.hpp"

struct Rect {
    float width;
    float height;
};

class Geo_Rect2D : public Geo_RenderObj {
public:
	Geo_Rect2D() : Geo_RenderObj() {}
    Geo_Rect2D(float sideLength){
        mVCount = 4; // Rectangle has 4 vertices
        mICount = 6; // Rectangle has 6 indices
        mRect.width = sideLength;
        mRect.height = sideLength;

        mVData = genVertices();
        mIData = genIndices();
		mTData = genTexCoords();
    }
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
