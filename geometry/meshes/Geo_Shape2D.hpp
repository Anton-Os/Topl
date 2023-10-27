#ifndef GEO_SHAPE2D_H

#include "Geo_Mesh.hpp"

class Geo_Shape : public Geo_Mesh {
public:
	Geo_Shape(Shape2D shape) 
	: Geo_Mesh(shape.segments + 1, shape.segments * 3) {
		_shape = shape;
		genVertices(); genIndices();
	}

	Geo_Shape(Shape2D shape, float z)
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

struct Geo_ShapeTriangle : public Geo_Shape {
	Geo_ShapeTriangle() : Geo_Shape({ 1.0, 3 }) {}
	Geo_ShapeTriangle(float radius) : Geo_Shape({ radius, 3 }) {}
	Geo_ShapeTriangle(float radius, float z) : Geo_Shape({ radius, 3 }, z) {}
};

struct Geo_ShapeQuad : public Geo_Shape {
	Geo_ShapeQuad() : Geo_Shape({ 1.0, 4 }) {}
	Geo_ShapeQuad(float radius) : Geo_Shape({ radius, 4 }) {}
	Geo_ShapeQuad(float radius, float z) : Geo_Shape({ radius, 4 }, z) {}
};

struct Geo_ShapeHex : public Geo_Shape {
	Geo_ShapeHex() : Geo_Shape({ 1.0, 6 }) {}
	Geo_ShapeHex(float radius) : Geo_Shape({ radius, 6 }) {}
	Geo_ShapeHex(float radius, float z) : Geo_Shape({ radius, 6 }, z) {}
};

struct Geo_ShapeCircle : public Geo_Shape {
	Geo_ShapeCircle() : Geo_Shape({ 1.0, CIRCLE_SEGMENTS }) {}
	Geo_ShapeCircle(float radius) : Geo_Shape({ radius, CIRCLE_SEGMENTS }) {}
	Geo_ShapeCircle(float radius, float z) : Geo_Shape({ radius, CIRCLE_SEGMENTS }, z) {}
};


#define GEO_SHAPE2D_H
#endif