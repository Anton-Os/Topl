#ifndef GEO_SHAPE2D_H

#include "Geo_Mesh.hpp"

class Geo_Surface : public Geo_Mesh {
public:
	Geo_Surface(Surface shape) : Geo_Mesh(shape.segments + 1, shape.segments * 3) {
		_shape = shape;
		genVertices(); genIndices();
	}

	Geo_Surface(Surface shape, float z) : Geo_Mesh(shape.segments + 1, shape.segments * 3) {
		_shape = shape;
		_depth = z;
		genVertices(); genIndices();
	}

	Geo_Surface(std::initializer_list<Vec3f> pointsSet) : Geo_Mesh(pointsSet.size(), pointsSet.size()){
		// TODO: Create Geo_Surface from points
		genVertices(); genIndices();
	}

	float getRadius(){ return _shape.radius; }
	float getSegments(){ return _shape.segments; }
private:
	void genVertices() override;
	void genIndices() override;

	Surface _shape;
	float _depth = DEFAULT_Z;
};

struct Geo_Triangle2D : public Geo_Surface {
	Geo_Triangle2D() : Geo_Surface({ 1.0, 3 }) {}
	Geo_Triangle2D(float radius) : Geo_Surface({ radius, 3 }) {}
	Geo_Triangle2D(float radius, float z) : Geo_Surface({ radius, 3 }, z) {}
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