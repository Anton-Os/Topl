#include "Geometry.hpp"

// See https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html for reference

enum SPHERE_Type {
    SPHERE_UV, // Default Surface Generation
    SPHERE_Icosphere,
    SPHERE_Quad,
    SPHERE_Goldberg
};

class Geo_SphereUV : public Geo_RenderObj {
public:
    Geo_SphereUV(NGon3D refShape)
    : Geo_RenderObj(refShape.xSegments * refShape.ySegments + 2, 0){
    // : Geo_RenderObj(refShape.xSegments * refShape.ySegments + 2, refShape.ySegments * 12) { // only indexes top and bottom
        mShape3D = refShape; // copy to internal data
        sphereType = SPHERE_UV;
        fillRenderObject();
    }

	float getRadius() const { return mShape3D.radius; }
private:
    void genVertices(Eigen::Vector3f* data) override;
    void genNormals(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;

    NGon3D mShape3D;
    SPHERE_Type sphereType; // Add support for multiple rendering types!
};