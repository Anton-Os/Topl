#ifndef GEO_SHAPE3D_H

#include "Geo_Mesh.hpp"

class Geo_Volume : public Geo_Mesh {
public:
	Geo_Volume(Shape shape, float depth);
	Geo_Volume(Vec3f* points, unsigned short pointCount, float depth);

	float getRadius(){ return _shape.radius; }
	float getSegments(){ return _shape.segments; }
	float getDepth(){ return _depth; }
private:
	Shape _shape;
	float _depth = 1.0F;
};

struct Geo_Trig3D : public Geo_Volume {
	Geo_Trig3D() : Geo_Volume({ 1.0, 3 }, 1.0F) {}
	Geo_Trig3D(float radius) : Geo_Volume({ radius, 3 }, 1.0F) {}
	Geo_Trig3D(float radius, float depth) : Geo_Volume({ radius, 3 }, depth) {}
};

struct Geo_Quad3D : public Geo_Volume {
	Geo_Quad3D() : Geo_Volume({ 1.0, 4 }, 1.0F) {}
	Geo_Quad3D(float radius) : Geo_Volume({ radius, 4 }, 1.0F) {}
	Geo_Quad3D(float radius, float depth) : Geo_Volume({ radius, 4 }, depth) {}
};

struct Geo_Hex3D : public Geo_Volume {
	Geo_Hex3D() : Geo_Volume({ 1.0, 6 }, 1.0F) {}
	Geo_Hex3D(float radius) : Geo_Volume({ radius, 6 }, 1.0F) {}
	Geo_Hex3D(float radius, float depth) : Geo_Volume({ radius, 6 }, depth) {}
};

struct Geo_Circle3D : public Geo_Volume {
	Geo_Circle3D() : Geo_Volume({ 1.0, CIRCLE_SEGMENTS }, 1.0F) {}
	Geo_Circle3D(float radius) : Geo_Volume({ radius, CIRCLE_SEGMENTS }, 1.0F) {}
	Geo_Circle3D(float radius, float depth) : Geo_Volume({ radius, CIRCLE_SEGMENTS }, depth) {}
};

struct Geo_Iter3D : public Geo_Volume { // expands depthwise into segments
	Geo_Iter3D(Shape shape, float depth, unsigned short iters);
	Geo_Iter3D(Vec3f* points, unsigned short pointCount, float depth, unsigned short iters);

	unsigned short _iters;
};


#define GEO_SHAPE3D_H
#endif