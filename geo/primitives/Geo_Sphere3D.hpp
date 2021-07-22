#include "Geometry.hpp"

// See https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html for reference

enum SPHERE_Type {
    SPHERE_UV, // Default Surface Generation
    SPHERE_Icosphere,
    SPHERE_Quad,
    SPHERE_Goldberg
};

class Geo_Sphere3D : public Geo_RenderObj {
public:
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
