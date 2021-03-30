#include "Geometry.hpp"

struct Sphere { // Same as Circle struct in Geo_NGon2D.hpp
    float radius;
    unsigned vSegments; // vertical segments
    unsigned hSegments; // horizontal segments
};

class Geo_Sphere3D : public Geo_RenderObj {
public:
    // Sphere Constructor
    Geo_Sphere3D(float radius, unsigned segments) 
    : Geo_RenderObj(segments * segments + 1, 40) { // TODO: Use proper values
        mSphere.radius = radius;
        mSphere.vSegments = segments;
        mSphere.hSegments = segments;

        fillRenderObject();
    }

	float getRadius() const { return mSphere.radius; }
private:
    void genVertices(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
    Sphere mSphere;
};
