#ifndef GEO_SHAPE3D_H

#include "Geo_Mesh.hpp"

class Geo_Volume : public Geo_Mesh {
public:
	Geo_Volume(Shape shape);
	Geo_Volume(Shape shape, float depth);
	Geo_Volume(std::initializer_list<Vec3f> pointsSet, float depth);

	float getRadius(){ return _shape.radius; }
	float getSegments(){ return _shape.segments; }
	float getDepth(){ return _depth; }
private:
	void genVertices() override;
	void genIndices() override;

	Shape _shape;
	float _depth = DEFAULT_Z;
};

struct Geo_Triangle3D : public Geo_Volume {
	Geo_Triangle3D() : Geo_Volume({ 1.0, 3 }) {}
	Geo_Triangle3D(float radius) : Geo_Volume({ radius, 3 }) {}
	Geo_Triangle3D(float radius, float depth) : Geo_Volume({ radius, 3 }, depth) {}
};

struct Geo_Quad3D : public Geo_Volume {
	Geo_Quad3D() : Geo_Volume({ 1.0, 4 }) {}
	Geo_Quad3D(float radius) : Geo_Volume({ radius, 4 }) {}
	Geo_Quad3D(float radius, float depth) : Geo_Volume({ radius, 4 }, depth) {}
};

struct Geo_Hex3D : public Geo_Volume {
	Geo_Hex3D() : Geo_Volume({ 1.0, 6 }) {}
	Geo_Hex3D(float radius) : Geo_Volume({ radius, 6 }) {}
	Geo_Hex3D(float radius, float depth) : Geo_Volume({ radius, 6 }, depth) {}
};

struct Geo_Circle3D : public Geo_Volume {
	Geo_Circle3D() : Geo_Volume({ 1.0, CIRCLE_SEGMENTS }) {}
	Geo_Circle3D(float radius) : Geo_Volume({ radius, CIRCLE_SEGMENTS }) {}
	Geo_Circle3D(float radius, float depth) : Geo_Volume({ radius, CIRCLE_SEGMENTS }, depth) {}
};

#define GEO_SHAPE3D_H
#endif