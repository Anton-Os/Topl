#ifndef Geo_Freeform_H

#include <utility>

#include "Geo_Mesh.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#define FREEFORM_LEVEL 0.5

struct ShapeFreeform {
    unsigned getCount() { return xDivs * yDivs * zDivs; }

    float radius;
    unsigned xDivs;
    unsigned yDivs;
    unsigned zDivs;
};

struct TrigForm {
    Vec3f pos1;
    Vec3f pos2;
    Vec3f pos3;
};

typedef bool (*fCullCallback)(Vec3f, double); // for culling freeform lattice
typedef TrigForm (*fSpawnCallback)(Vec3f); // for spawning vertices

// Functions

bool freeformCull_ball(Vec3f input, double level);
TrigForm freeformSpawn_trigs(Vec3f input);

DllExport Geo_Vertex* genFreeform_vertices(ShapeFreeform shape, fCullCallback callback);
DllExport unsigned* genFreeform_indices(ShapeFreeform shape, unsigned count);

#ifdef __cplusplus
class Geo_Freeform : public Geo_Mesh {
public:
    DllExport Geo_Freeform(ShapeFreeform shape);
    DllExport Geo_Freeform(ShapeFreeform shape, fCullCallback callback);
    // DllExport Geo_Freeform(ShapeFreeform shape, fCullCallback cullCallback, fSpawnCallback spawnCallback);
private:
    ShapeFreeform _shape;
    double _level = FREEFORM_LEVEL;
};

}
#endif

#define Geo_Freeform_H
#endif