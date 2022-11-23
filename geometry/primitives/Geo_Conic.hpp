#include "Geometry.hpp"

class Geo_Conic : public Geo_Renderable {
public:
	Geo_Conic(Shape2D shape)
	: Geo_Renderable(shape.segments + 2, shape.segments * 6) {
		_shape = shape;
		_apex = Vec3f({ 0.0f, 0.0f, shape.radius + DEFAULT_Z }); // default apex
		genVertices(); genIndices();
	}

	Geo_Conic(Shape2D shape, Vec3f apex)
	: Geo_Renderable(shape.segments + 2, shape.segments * 6) {
		_shape = shape;
		_apex = apex; // custom apex
		genVertices(); genIndices();
	}

	float getRadius() const { return _shape.radius; }
	unsigned getSegments() const { return _shape.segments; }
	Vec3f getApex() const { return _apex; }
private:
	void genVertices() override;
	void genIndices() override;

	Shape2D _shape;
	Vec3f _apex;
};

struct Geo_ConicTriangle : public Geo_Conic {
	Geo_ConicTriangle() : Geo_Conic({ 1.0, 3 }) {}
	Geo_ConicTriangle(float radius) : Geo_Conic({ radius, 3 }) {}
	Geo_ConicTriangle(float radius, Vec3f apex) : Geo_Conic({ radius, 3 }, apex) {}
};

struct Geo_ConicSquare : public Geo_Conic { // Pyramid
	Geo_ConicSquare() : Geo_Conic({ 1.0, 4 }) {}
	Geo_ConicSquare(float radius) : Geo_Conic({ radius, 4 }) {}
	Geo_ConicSquare(float radius, Vec3f apex) : Geo_Conic({ radius, 4 }, apex) {}
};

struct Geo_ConicHex : public Geo_Conic {
	Geo_ConicHex() : Geo_Conic({ 1.0, 6 }) {}
	Geo_ConicHex(float radius) : Geo_Conic({ radius, 6 }) {}
	Geo_ConicHex(float radius, Vec3f apex) : Geo_Conic({ radius, 6 }, apex) {}
};

struct Geo_ConicCircle : public Geo_Conic { // Conic
	Geo_ConicCircle() : Geo_Conic({ 1.0, CIRCLE_SEGMENTS }) {}
	Geo_ConicCircle(float radius) : Geo_Conic({ radius, CIRCLE_SEGMENTS }) {}
	Geo_ConicCircle(float radius, Vec3f apex) : Geo_Conic({ radius, CIRCLE_SEGMENTS }, apex) {}
};