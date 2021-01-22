#include "Geometry.hpp"

struct Circle {
    float radius;
    unsigned segments;
};

class Geo_Sphere2D : Geo_RenderObj {
public:
    Geo_Sphere2D(float radius, unsigned segments) : Geo_RenderObj() {
        mVCount = segments + 1; // Number of segments + center point
        mICount = segments * 3; // Rectangle has 6 indices
        mCircle.radius = radius;
        mCircle.segments = segments;

        mVData = genVertices();
		mTData = genTexCoords();
        mIData = genIndices();
    }
private:
    Eigen::Vector3f* genVertices() override;
	Eigen::Vector2f* genTexCoords() override;
    unsigned* genIndices() override;
    Circle mCircle;
};