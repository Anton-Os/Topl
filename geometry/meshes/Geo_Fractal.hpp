#ifndef GEO_FRACTAL_H

#include <utility>

#include "Geo_Mesh.hpp"

// Classes

struct ShapeFractal {
    unsigned getCount() { return xDivs * yDivs * zDivs; }

    float radius;
    unsigned xDivs;
    unsigned yDivs;
    unsigned zDivs;
};

struct TrigFractal {
    Vec3f pos1;
    Vec3f pos2;
    Vec3f pos3;
};

typedef bool (*fCullCallback)(Vec3f, double); // for culling fractal lattice
typedef TrigFractal (*fSpawnCallback)(Vec3f); // for spawning vertices

#define FRACTAL_LEVEL 0.5

#ifdef __cplusplus
extern "C" {

class Geo_Fractal : public Geo_Mesh {
public:
    DllExport Geo_Fractal(ShapeFractal shape);
    DllExport Geo_Fractal(ShapeFractal shape, fCullCallback callback);
    // DllExport Geo_Fractal(ShapeFractal shape, fCullCallback cullCallback, fSpawnCallback spawnCallback);
private:
    ShapeFractal _shape;
    double _level = FRACTAL_LEVEL;
};
#endif

// Functions

bool fractalCull_ball(Vec3f input, double level);
TrigFractal fractalSpawn_trigs(Vec3f input);

DllExport Geo_Vertex* genFractal_vertices(ShapeFractal shape, fCullCallback callback);
DllExport unsigned* genFractal_indices(ShapeFractal shape, unsigned count);

#ifdef __cplusplus
}
#endif

#define GEO_FRACTAL_H
#endif