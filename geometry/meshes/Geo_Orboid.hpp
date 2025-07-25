#ifndef GEO_ORBOID_H

#include "Geo_Mesh.hpp"

// Orb

Geo_Vertex* genOrb_vertices(Shape3D shape);
unsigned* genOrb_indices(Shape3D shape);

#ifdef __cplusplus
class Geo_Orb : public Geo_Mesh { // See https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html for reference
public:
	Geo_Orb(float size);
	Geo_Orb(Shape3D shape);

	float getRadius() const { return _shape.radius; }
private:
	void init();

	Shape3D _shape;
};

struct Geo_TrigOrb : public Geo_Orb {
	Geo_TrigOrb() : Geo_Orb({ 1.0, 3, 3 }){}
	Geo_TrigOrb(float radius) : Geo_Orb({ radius, 3, 3 }){}
};

struct Geo_QuadOrb : public Geo_Orb {
	Geo_QuadOrb() : Geo_Orb({ 1.0, 4, 4 }){}
	Geo_QuadOrb(float radius) : Geo_Orb({ radius, 4, 4 }){}
};

struct Geo_HexOrb : public Geo_Orb {
	Geo_HexOrb() : Geo_Orb({ 1.0, 6, 6 }){}
	Geo_HexOrb(float radius) : Geo_Orb({ radius, 6, 6 }){}
};

struct Geo_OctOrb : public Geo_Orb {
	Geo_OctOrb() : Geo_Orb({ 1.0, 8, 8 }){}
	Geo_OctOrb(float radius) : Geo_Orb({ radius, 8, 8 }){}
};

struct Geo_DecOrb : public Geo_Orb {
	Geo_DecOrb() : Geo_Orb({ 1.0, 10, 10 }){}
	Geo_DecOrb(float radius) : Geo_Orb({ radius, 10, 10 }){}
};
#endif

// Torus

Geo_Vertex* genTorus_vertices(Shape3D shape, float d);
unsigned* genTorus_indices(Shape3D shape);

#ifdef __cplusplus
class Geo_Torus : public Geo_Mesh {
public:
	Geo_Torus(float diameter);
	Geo_Torus(float diameter, Shape3D shape);
private:
	float _diameter = 0.5F;
	Shape3D _shape;
};
#endif

#define GEO_ORBOID_H
#endif