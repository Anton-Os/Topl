#include "Geometry.hpp"

struct Circle {
    float radius;
    unsigned segments;
};

class Geo_Sphere2D : public Geo_RenderObj {
public:
    // Empty constructor
    Geo_Sphere2D() : Geo_RenderObj(){}
    // Circle constructor
    Geo_Sphere2D(float radius, unsigned segments) 
    : Geo_RenderObj(segments + 1, segments * 3) { // Vertex count is + 1 to include center point
        mCircle.radius = radius;
        mCircle.segments = segments;

        fillRenderObject();
    }

    float getRadius() const { return mCircle.radius; }
private:
    Eigen::Vector3f* genVertices() override;
	Eigen::Vector2f* genTexCoords() override;
    unsigned* genIndices() override;
    Circle mCircle;
};