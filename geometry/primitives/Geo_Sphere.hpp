#include "Geometry.hpp"

// See https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html for reference

/* class Geo_Sphere : public Geo_RenderObj {
public:
    Geo_Sphere(NGon3D refShape)
    : Geo_RenderObj(
        ((refShape.xSegments + 1) * (refShape.ySegments + 1)), // slices and stacks
        ((refShape.xSegments + 1) * (refShape.ySegments + 1)) * 6 // Index Count
    ){
        _shape3D = refShape;
        init();
    }

	float getRadius() const { return _shape3D.radius; }
private:
    void genPos(Vec3f* data) override;
    void genNormals(Vec3f* data) override;
	void genTexCoords(Vec2f* data) override;
    void genIndices(unsigned* data) override;

    NGon3D _shape3D;
}; */

class Geo_Sphere : public Geo_Renderable {
public:
	Geo_Sphere(Shape3D shape)
	: Geo_Renderable(
		((shape.xSegments + 1) * (shape.ySegments + 1)), // slices and stacks
		((shape.xSegments + 1) * (shape.ySegments + 1)) * 6 // index count
	){
		_shape = shape;

		genVertices(); genIndices();
	}

	float getRadius() const { return _shape.radius; }
private:
	void genVertices() override;
	void genIndices() override;

	Shape3D _shape;
};
