#ifndef GEO_FLAT_H

#include "Geo_Mesh.hpp"

class Geo_Flat : public Geo_Mesh {
public:
	Geo_Flat(Shape2D shape) 
	: Geo_Mesh(shape.segments + 1, shape.segments * 3) {
		_shape = shape;
		genVertices(); genIndices();
	}

	Geo_Flat(Shape2D shape, float z)
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

struct Geo_FlatTriangle : public Geo_Flat {
	Geo_FlatTriangle() : Geo_Flat({ 1.0, 3 }) {}
	Geo_FlatTriangle(float radius) : Geo_Flat({ radius, 3 }) {}
	Geo_FlatTriangle(float radius, float z) : Geo_Flat({ radius, 3 }, z) {}
};

struct Geo_FlatSquare : public Geo_Flat {
	Geo_FlatSquare() : Geo_Flat({ 1.0, 4 }) {}
	Geo_FlatSquare(float radius) : Geo_Flat({ radius, 4 }) {}
	Geo_FlatSquare(float radius, float z) : Geo_Flat({ radius, 4 }, z) {}
};

struct Geo_FlatHex : public Geo_Flat {
	Geo_FlatHex() : Geo_Flat({ 1.0, 6 }) {}
	Geo_FlatHex(float radius) : Geo_Flat({ radius, 6 }) {}
	Geo_FlatHex(float radius, float z) : Geo_Flat({ radius, 6 }, z) {}
};

struct Geo_FlatCircle : public Geo_Flat {
	Geo_FlatCircle() : Geo_Flat({ 1.0, CIRCLE_SEGMENTS }) {}
	Geo_FlatCircle(float radius) : Geo_Flat({ radius, CIRCLE_SEGMENTS }) {}
	Geo_FlatCircle(float radius, float z) : Geo_Flat({ radius, CIRCLE_SEGMENTS }, z) {}
};


#define GEO_FLAT_H
#endif