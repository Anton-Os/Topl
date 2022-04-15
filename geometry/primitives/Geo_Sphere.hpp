#include "Geometry.hpp"

// See https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html for reference

class Geo_Sphere : public Geo_RenderObj {
public:
    Geo_Sphere(NGon3D refShape)
    : Geo_RenderObj(
        (refShape.xSegments * refShape.ySegments) + 2 // slices and stacks + top and bottom vertices
        // (refShape.ySegments * 6) + ((refShape.xSegments * refShape.ySegments) * 6) // Index Count
    ){
        _shape3D = refShape;
        fillRenderObj();
    }

	float getRadius() const { return _shape3D.radius; }
private:
    void genPos(Vec3f* data) override;
    void genNormals(Vec3f* data) override;
	void genTexCoords(Vec2f* data) override;
    void genIndices(unsigned* data) override;

    NGon3D _shape3D;
};

class Geo_IcoSphere : public Geo_RenderObj {
    Geo_IcoSphere(NGon3D refShape)
    : Geo_RenderObj( 1, 1 ){ // include correct vertex and index count
        _shape3D = refShape;
        fillRenderObj();
    }

	float getRadius() const { return _shape3D.radius; }
private:
    void genPos(Vec3f* data) override;
    void genNormals(Vec3f* data) override;
	void genTexCoords(Vec2f* data) override;
    void genIndices(unsigned* data) override;

    NGon3D _shape3D;
};
