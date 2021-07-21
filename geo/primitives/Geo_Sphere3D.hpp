#include "Geometry.hpp"

class Geo_Sphere3D : public Geo_RenderObj {
public:
    // Sphere Constructor
    // Geo_Sphere3D(float radius, unsigned segments)
    Geo_Sphere3D(NGon3D refShape)
    : Geo_RenderObj(mShape3D.xSegments * mShape3D.ySegments + 1, 40) { // TODO: Use proper values!
        mShape3D = refShape; // copy to internal data
        fillRenderObject();
    }

	float getRadius() const { return mShape3D.radius; }
private:
    void genVertices(Eigen::Vector3f* data) override;
    void genNormals(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
    NGon3D mShape3D;
};
