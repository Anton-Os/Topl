#include "Geometry.hpp"

struct NGon {
    float radius;
    unsigned segments;
};

class Geo_NGon2D : public Geo_RenderObj {
public:
    // Empty constructor
    Geo_NGon2D() : Geo_RenderObj(){}
    // Equalateral triangle constructor
    Geo_NGon2D(float radius)
    : Geo_RenderObj(3 + 1, 3 * 3) {
        mNGon.radius = radius;
        mNGon.segments = 3;
    }
    // Circle constructor
    Geo_NGon2D(float radius, unsigned segments) 
    : Geo_RenderObj(segments + 1, segments * 3) { // Vertex count is + 1 to include center point
        mNGon.radius = radius;
        mNGon.segments = segments;

        fillRenderObject();
    }

    float getRadius() const { return mNGon.radius; }
    unsigned getSegments() const { return mNGon.segments; }
private:
    void genVertices(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
    NGon mNGon;
};