#ifndef GEO_CONIC_H

#include "Geo_Mesh.hpp"

Geo_Vertex* genCone_vertices(Shape2D shape, Vec3f apex);
Geo_Vertex* genConeP_vertices(vertex_cptr_t points, unsigned c, Vec3f apex);
unsigned* genCone_indices(Shape2D shape);

class Geo_Cone : public Geo_Mesh {
public:
	Geo_Cone(Shape2D shape, Vec3f apex);
	Geo_Cone(vertex_cptr_t points, unsigned short pointCount, Vec3f apex);

	float getRadius() const { return _shape.radius; }
	unsigned short getSegments() const { return _shape.segments; }
	Vec3f getApex() const { return _apex; }
private:
	Shape2D  _shape;
	Vec3f _apex;
};

struct Geo_TrigCone : public Geo_Cone {
	Geo_TrigCone() : Geo_Cone({ 1.0, 3 }, Vec3f({ 0.0f, 0.0f, 1.0F + DEFAULT_Z })) {}
	Geo_TrigCone(float radius) : Geo_Cone({ radius, 3 }, Vec3f({ 0.0f, 0.0f, radius + DEFAULT_Z })) {}
	Geo_TrigCone(float radius, Vec3f apex) : Geo_Cone({ radius, 3 }, apex) {}
};

struct Geo_QuadCone : public Geo_Cone { // Pyramid
	Geo_QuadCone() : Geo_Cone({ 1.0, 4 }, Vec3f({ 0.0f, 0.0f, 1.0F + DEFAULT_Z })) {}
	Geo_QuadCone(float radius) : Geo_Cone({ radius, 4 }, Vec3f({ 0.0f, 0.0f, radius + DEFAULT_Z })) {}
	Geo_QuadCone(float radius, Vec3f apex) : Geo_Cone({ radius, 4 }, apex) {}
};

struct Geo_HexCone : public Geo_Cone {
	Geo_HexCone() : Geo_Cone({ 1.0, 6 }, Vec3f({ 0.0f, 0.0f, 1.0F + DEFAULT_Z })) {}
	Geo_HexCone(float radius) : Geo_Cone({ radius, 6 }, Vec3f({ 0.0f, 0.0f, radius + DEFAULT_Z })) {}
	Geo_HexCone(float radius, Vec3f apex) : Geo_Cone({ radius, 6 }, apex) {}
};

struct Geo_CircleCone : public Geo_Cone { // Cone
	Geo_CircleCone() : Geo_Cone({ 1.0, CIRCLE_SEGMENTS }, Vec3f({ 0.0f, 0.0f, 1.0F + DEFAULT_Z })) {}
	Geo_CircleCone(float radius) : Geo_Cone({ radius, CIRCLE_SEGMENTS }, Vec3f({ 0.0f, 0.0f, radius + DEFAULT_Z })) {}
	Geo_CircleCone(float radius, Vec3f apex) : Geo_Cone({ radius, CIRCLE_SEGMENTS }, apex) {}
};

struct Geo_ExtCone : public Geo_Cone { // expands base into repeating portions
    // Geo_ExtCone(Shape2D shape, Vec3f apex); // one iteration
    Geo_ExtCone(Shape2D shape, Vec3f apex, unsigned short iters);
    // Geo_ExtCone(vertex_cptr_t points, unsigned short pointCount, Vec3f apex);
    Geo_ExtCone(vertex_cptr_t points, unsigned short pointCount, Vec3f apex, unsigned short iters);

	unsigned short _iters;
};

#define GEO_CONIC_H
#endif
