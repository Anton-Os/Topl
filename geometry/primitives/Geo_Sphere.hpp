#include "Geometry.hpp"

// See https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html for reference

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
