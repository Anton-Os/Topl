#ifndef GEO_SHAPE2D_H

#include "Geo_Mesh.hpp"

class Geo_Shape2D : public Geo_Mesh {
public:
	Geo_Shape2D(Shape2D shape) 
	: Geo_Mesh(shape.segments + 1, shape.segments * 3) {
		_shape = shape;
		genVertices(); genIndices();
	}

	Geo_Shape2D(Shape2D shape, float z)
	: Geo_Mesh(shape.segments + 1, shape.segments * 3) {
		_shape = shape;
		_depth = z;
		genVertices(); genIndices();
	}

	float getRadius(){ return _shape.radius; }
	float getSegments(){ return _shape.segments; }
private:
	void genVertices() override;
	void genIndices() override;

	Shape2D _shape;
	float _depth = DEFAULT_Z;
};

struct Geo_Triangle2D : public Geo_Shape2D {
	Geo_Triangle2D() : Geo_Shape2D({ 1.0, 3 }) {}
	Geo_Triangle2D(float radius) : Geo_Shape2D({ radius, 3 }) {}
	Geo_Triangle2D(float radius, float z) : Geo_Shape2D({ radius, 3 }, z) {}
};

struct Geo_Quad2D : public Geo_Shape2D {
	Geo_Quad2D() : Geo_Shape2D({ 1.0, 4 }) {}
	Geo_Quad2D(float radius) : Geo_Shape2D({ radius, 4 }) {}
	Geo_Quad2D(float radius, float z) : Geo_Shape2D({ radius, 4 }, z) {}
};

struct Geo_Hex2D : public Geo_Shape2D {
	Geo_Hex2D() : Geo_Shape2D({ 1.0, 6 }) {}
	Geo_Hex2D(float radius) : Geo_Shape2D({ radius, 6 }) {}
	Geo_Hex2D(float radius, float z) : Geo_Shape2D({ radius, 6 }, z) {}
};

struct Geo_Circle2D : public Geo_Shape2D {
	Geo_Circle2D() : Geo_Shape2D({ 1.0, CIRCLE_SEGMENTS }) {}
	Geo_Circle2D(float radius) : Geo_Shape2D({ radius, CIRCLE_SEGMENTS }) {}
	Geo_Circle2D(float radius, float z) : Geo_Shape2D({ radius, CIRCLE_SEGMENTS }, z) {}
};

#define GEO_SHAPE2D_H
#endif