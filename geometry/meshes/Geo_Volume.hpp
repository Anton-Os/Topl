#ifndef GEO_VOLUME_H

#include "Geo_Mesh.hpp"

Geo_Vertex* genVolume_vertices(Shape2D shape, float d);
Geo_Vertex* genVolumeP_vertices(vertex_cptr_t points, unsigned c, float d);
unsigned* genVolume_indices(Shape2D shape);

#ifdef __cplusplus
class Geo_Volume : public Geo_Mesh {
public:
	Geo_Volume(Shape2D shape, float depth);
	Geo_Volume(vertex_cptr_t points, unsigned short pointCount, float depth);

	float getRadius() { return _shape.radius; }
	unsigned short getSegments() { return _shape.segments; }
	float getDepth() { return _depth; }
private:
	Shape2D  _shape;
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

struct Geo_Ext3D : public Geo_Volume { // expands depthwise into segments
	// Geo_Ext3D(Shape2D shape, float depth); // one iteration
	Geo_Ext3D(Shape2D shape, float depth, unsigned short iters);
	// Geo_Ext3D(vertex_cptr_t points, unsigned short pointCount, float depth); // one iteration
	Geo_Ext3D(vertex_cptr_t points, unsigned short pointCount, float depth, unsigned short iters);

	unsigned short _iters;
};
#endif

#define GEO_VOLUME_H
#endif
