#ifndef GEO_SURFACE_H

#include "Geo_Mesh.hpp"

#ifdef __cplusplus
extern "C" {
#endif

DllExport Geo_Vertex* genSurface_vertices(Shape2D shape, float z);
DllExport Geo_Vertex* genSurface_vert(vertex_cptr_t points, unsigned c, float z);
DllExport unsigned* genSurface_indices(Shape2D shape);

#ifdef __cplusplus
class Geo_Surface : public Geo_Mesh {
public:
	DllExport Geo_Surface(Shape2D shape, float z);
	DllExport Geo_Surface(vertex_cptr_t points, unsigned short pointCount, float z);

	float getRadius(){ return _shape.radius; }
	unsigned short getSegments(){ return _shape.segments; }
private:
	Shape2D  _shape = { 0.0F, 0 };
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

struct Geo_Ext2D : public Geo_Surface { // expands surface into subdivided portions
    // Geo_Ext2D(Shape2D shape, float z); // one iteration
    DllExport Geo_Ext2D(Shape2D shape, float z, unsigned short iters);
    // Geo_Ext2D(vertex_cptr_t points, unsigned short pointCount); // one iteration
	DllExport Geo_Ext2D(vertex_cptr_t points, unsigned short pointCount, unsigned short iters);

	unsigned short _iters;
}; 
}
#endif

#define GEO_SURFACE_H
#endif
