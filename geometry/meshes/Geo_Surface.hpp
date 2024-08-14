#ifndef GEO_SHAPE2D_H

#include "Geo_Mesh.hpp"

class Geo_Surface : public Geo_Mesh {
public:
	Geo_Surface(Shape shape, float z);
	Geo_Surface(Vec3f* points, unsigned short pointCount);

	float getRadius(){ return _shape.radius; }
	float getSegments(){ return _shape.segments; }
private:

	Shape _shape = { 0.0F, 0 };
	float _depth = DEFAULT_Z;
};

struct Geo_Trig2D : public Geo_Surface {
	Geo_Trig2D() : Geo_Surface({ 1.0, 3 }, DEFAULT_Z) {}
	Geo_Trig2D(float radius) : Geo_Surface({ radius, 3 }, DEFAULT_Z) {}
	Geo_Trig2D(float radius, float z) : Geo_Surface({ radius, 3 }, z) {}
};

struct Geo_Quad2D : public Geo_Surface {
	Geo_Quad2D() : Geo_Surface({ 1.0, 4 }, DEFAULT_Z) {}
	Geo_Quad2D(float radius) : Geo_Surface({ radius, 4 }, DEFAULT_Z) {}
	Geo_Quad2D(float radius, float z) : Geo_Surface({ radius, 4 }, z) {}
};

struct Geo_Hex2D : public Geo_Surface {
	Geo_Hex2D() : Geo_Surface({ 1.0, 6 }, DEFAULT_Z) {}
	Geo_Hex2D(float radius) : Geo_Surface({ radius, 6 }, DEFAULT_Z) {}
	Geo_Hex2D(float radius, float z) : Geo_Surface({ radius, 6 }, z) {}
};

struct Geo_Circle2D : public Geo_Surface {
	Geo_Circle2D() : Geo_Surface({ 1.0, CIRCLE_SEGMENTS }, DEFAULT_Z) {}
	Geo_Circle2D(float radius) : Geo_Surface({ radius, CIRCLE_SEGMENTS }, DEFAULT_Z) {}
	Geo_Circle2D(float radius, float z) : Geo_Surface({ radius, CIRCLE_SEGMENTS }, z) {}
};

struct Geo_Iter2D : public Geo_Surface { // expands surface into subdivided portions
	Geo_Iter2D(Shape shape, float z, unsigned short iters);
	Geo_Iter2D(Vec3f* points, unsigned short pointCount, unsigned short iters);

	unsigned short _iters;
};

#define GEO_SHAPE2D_H
#endif
