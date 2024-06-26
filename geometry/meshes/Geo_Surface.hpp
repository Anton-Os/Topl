#ifndef GEO_SHAPE2D_H

#include "Geo_Mesh.hpp"

class Geo_Surface : public Geo_Mesh {
public:
	Geo_Surface(Shape shape);
	Geo_Surface(Shape shape, float z);
	Geo_Surface(std::initializer_list<Vec3f> pointsSet);

	float getRadius(){ return _shape.radius; }
	float getSegments(){ return _shape.segments; }
private:
	void genVertices() override;
	void genIndices() override;

	Shape _shape;
	float _depth = DEFAULT_Z;
};

struct Geo_Trig2D : public Geo_Surface {
	Geo_Trig2D() : Geo_Surface({ 1.0, 3 }) {}
	Geo_Trig2D(float radius) : Geo_Surface({ radius, 3 }) {}
	Geo_Trig2D(float radius, float z) : Geo_Surface({ radius, 3 }, z) {}
};

struct Geo_Quad2D : public Geo_Surface {
	Geo_Quad2D() : Geo_Surface({ 1.0, 4 }) {}
	Geo_Quad2D(float radius) : Geo_Surface({ radius, 4 }) {}
	Geo_Quad2D(float radius, float z) : Geo_Surface({ radius, 4 }, z) {}
};

struct Geo_Hex2D : public Geo_Surface {
	Geo_Hex2D() : Geo_Surface({ 1.0, 6 }) {}
	Geo_Hex2D(float radius) : Geo_Surface({ radius, 6 }) {}
	Geo_Hex2D(float radius, float z) : Geo_Surface({ radius, 6 }, z) {}
};

struct Geo_Circle2D : public Geo_Surface {
	Geo_Circle2D() : Geo_Surface({ 1.0, CIRCLE_SEGMENTS }) {}
	Geo_Circle2D(float radius) : Geo_Surface({ radius, CIRCLE_SEGMENTS }) {}
	Geo_Circle2D(float radius, float z) : Geo_Surface({ radius, CIRCLE_SEGMENTS }, z) {}
};

#define GEO_SHAPE2D_H
#endif