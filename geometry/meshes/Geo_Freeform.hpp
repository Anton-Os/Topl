#ifndef GEO_FREEFORM_H

#include <utility>

#include "Geo_Mesh.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#define FREEFORM_LEVEL 0.25F

struct ShapeFreeform {
    unsigned getCount() { return xDivs * yDivs * zDivs; }

    float radius;
    unsigned xDivs;
    unsigned yDivs;
    unsigned zDivs;
};

struct TrigForm { Vec3f positions[3]; };

typedef bool (*fCullCallback)(Vec3f, double); // for culling freeform lattice
typedef TrigForm (*fSpawnCallback)(Vec3f); // for spawning vertices

// Cull Functions
bool freeformCull_none(Vec3f input, double level);
bool freeformCull_ball(Vec3f input, double level);

// Spawn Functions
TrigForm freeformSpawn_trigs(Vec3f input);

// Generative Functionis
DllExport Geo_Vertex* genFreeform_vertices(ShapeFreeform shape, fCullCallback callback);
DllExport unsigned* genFreeform_indices(ShapeFreeform shape, unsigned count);

#ifdef __cplusplus

class Geo_Freeform : public Geo_Mesh {
public:
    DllExport Geo_Freeform(ShapeFreeform shape);
    DllExport Geo_Freeform(ShapeFreeform shape, fSpawnCallback spawnCallback, fCullCallback callback);
private:
    ShapeFreeform _shape;
    double _level = FREEFORM_LEVEL;
};

}
#endif

#define GEO_FREEFORM_H
#endif