#ifndef GEO_SPHERE_H

#include "Geo_Mesh.hpp"

// See https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html for reference

class Geo_Orb : public Geo_Mesh {
public:
	Geo_Orb() : Geo_Mesh(
		((CIRCLE_SEGMENTS + 1) * (CIRCLE_SEGMENTS + 1)), // slices and stacks
		((CIRCLE_SEGMENTS + 1) * (CIRCLE_SEGMENTS + 1)) * 6 // index count
	){
		_shape = { 1.0f, CIRCLE_SEGMENTS, CIRCLE_SEGMENTS };
		init();
	}

	Geo_Orb(float size) : Geo_Mesh(
		((CIRCLE_SEGMENTS + 1) * (CIRCLE_SEGMENTS + 1)), // slices and stacks
		((CIRCLE_SEGMENTS + 1) * (CIRCLE_SEGMENTS + 1)) * 6 // index count
	){
		_shape = { size, CIRCLE_SEGMENTS, CIRCLE_SEGMENTS };
		init();
	}

	Geo_Orb(Shape3D shape) : Geo_Mesh(
		((shape.xSegs + 1) * (shape.ySegs + 1)), // slices and stacks
		((shape.xSegs + 1) * (shape.ySegs + 1)) * 6 // index count
	){
		_shape = shape;
		init();
	}

	float getRadius() const { return _shape.radius; }
private:
	void init();

	Shape3D _shape;
};

#define GEO_SPHERE_H
#endif