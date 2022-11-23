#include "Geometry.hpp"

class Geo_Extruded : public Geo_Renderable {
public:
	Geo_Extruded(Shape2D shape)
	: Geo_Renderable((shape.segments + 1) * 2, shape.segments * 12) {
		_shape = shape;
		_depth = shape.radius;
		genVertices(); genIndices();
	}

	Geo_Extruded(Shape2D shape, float depth) 
	: Geo_Renderable((shape.segments + 1) * 2, shape.segments * 12) {
		_shape = shape;
		_depth = depth;
		genVertices(); genIndices();
	}
private:
	void genVertices() override;
	void genIndices() override;

	Shape2D _shape;
	float _depth = DEFAULT_Z;
};

struct Geo_ExtrudedTriangle : public Geo_Extruded {
	Geo_ExtrudedTriangle() : Geo_Extruded({ 1.0, 3 }) {}
	Geo_ExtrudedTriangle(float radius) : Geo_Extruded({ radius, 3 }) {}
	Geo_ExtrudedTriangle(float radius, float depth) : Geo_Extruded({ radius, 3 }, depth) {}
};

struct Geo_ExtrudedSquare : public Geo_Extruded { // Box
	Geo_ExtrudedSquare() : Geo_Extruded({ 1.0, 4 }) {}
	Geo_ExtrudedSquare(float radius) : Geo_Extruded({ radius, 4 }) {}
	Geo_ExtrudedSquare(float radius, float depth) : Geo_Extruded({ radius, 4 }, depth) {}
};

struct Geo_ExtrudedHex : public Geo_Extruded {
	Geo_ExtrudedHex() : Geo_Extruded({ 1.0, 6 }) {}
	Geo_ExtrudedHex(float radius) : Geo_Extruded({ radius, 6 }) {}
	Geo_ExtrudedHex(float radius, float depth) : Geo_Extruded({ radius, 6 }, depth) {}
};

struct Geo_ExtrudedCircle : public Geo_Extruded { // Cylinder
	Geo_ExtrudedCircle() : Geo_Extruded({ 1.0, CIRCLE_SEGMENTS }) {}
	Geo_ExtrudedCircle(float radius) : Geo_Extruded({ radius, CIRCLE_SEGMENTS }) {}
	Geo_ExtrudedCircle(float radius, float depth) : Geo_Extruded({ radius, CIRCLE_SEGMENTS }, depth) {}
};