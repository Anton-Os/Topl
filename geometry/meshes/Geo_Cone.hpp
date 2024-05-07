#ifndef GEO_CONIC_H

#include "Geo_Mesh.hpp"

class Geo_Cone : public Geo_Mesh {
public:
	Geo_Cone(Shape shape);
	Geo_Cone(Shape shape, Vec3f apex);
	Geo_Cone(std::initializer_list<Vec3f> pointsSet, Vec3f apex);

	float getRadius() const { return _shape.radius; }
	unsigned getSegments() const { return _shape.segments; }
	Vec3f getApex() const { return _apex; }
private:
	void genVertices() override;
	void genIndices() override;

	Shape _shape;
	Vec3f _apex;
};

struct Geo_TriangleCone : public Geo_Cone {
	Geo_TriangleCone() : Geo_Cone({ 1.0, 3 }) {}
	Geo_TriangleCone(float radius) : Geo_Cone({ radius, 3 }) {}
	Geo_TriangleCone(float radius, Vec3f apex) : Geo_Cone({ radius, 3 }, apex) {}
};

struct Geo_QuadCone : public Geo_Cone { // Pyramid
	Geo_QuadCone() : Geo_Cone({ 1.0, 4 }) {}
	Geo_QuadCone(float radius) : Geo_Cone({ radius, 4 }) {}
	Geo_QuadCone(float radius, Vec3f apex) : Geo_Cone({ radius, 4 }, apex) {}
};

struct Geo_HexCone : public Geo_Cone {
	Geo_HexCone() : Geo_Cone({ 1.0, 6 }) {}
	Geo_HexCone(float radius) : Geo_Cone({ radius, 6 }) {}
	Geo_HexCone(float radius, Vec3f apex) : Geo_Cone({ radius, 6 }, apex) {}
};

struct Geo_CircleCone : public Geo_Cone { // Cone
	Geo_CircleCone() : Geo_Cone({ 1.0, CIRCLE_SEGMENTS }) {}
	Geo_CircleCone(float radius) : Geo_Cone({ radius, CIRCLE_SEGMENTS }) {}
	Geo_CircleCone(float radius, Vec3f apex) : Geo_Cone({ radius, CIRCLE_SEGMENTS }, apex) {}
};

#define GEO_CONIC_H
#endif