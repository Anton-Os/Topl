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
    : Geo_RenderObj(
        (refShape.xSegments * refShape.ySegments) + 2 - refShape.ySegments, // Vertex Count
        (refShape.ySegments * 6) + ((refShape.xSegments * refShape.ySegments) * 6) // Index Count
    ){
    // : Geo_RenderObj(refShape.xSegments * refShape.ySegments + 2, refShape.ySegments * 12) { // only indexes top and bottom
        _shape3D = refShape; // copy to internal data
        _sphereType = SPHERE_UV;
        fillRenderObj();
    }

	float getRadius() const { return _shape3D.radius; }
private:
    void genPos(Vec3f* data) override;
    void genNormals(Vec3f* data) override;
	void genTexCoords(Vec2f* data) override;
    void genIndices(unsigned* data) override;

    NGon3D _shape3D;
    SPHERE_Type _sphereType; // Add support for multiple rendering types!
};
