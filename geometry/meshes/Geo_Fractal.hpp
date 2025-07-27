#ifndef GEO_FRACTAL_H

#include "Geo_Mesh.hpp"

// Classes

struct ShapeFractal {
    unsigned getCount() { return xDivs * yDivs * zDivs; }

    float radius;
    unsigned xDivs;
    unsigned yDivs;
    unsigned zDivs;
};

typedef bool (*fractalCallback)(Vec3f);

#ifdef __cplusplus
extern "C" {

class Geo_Fractal : public Geo_Mesh {
public:
    Geo_Fractal(ShapeFractal shape);
    Geo_Fractal(ShapeFractal shape, fractalCallback callback);
private:
    ShapeFractal _shape;
};
#endif

// Functions

bool fractalBall(Vec3f input);

Geo_Vertex* genFractal_vertices(ShapeFractal shape, fractalCallback callback);
unsigned* genFractal_indices(ShapeFractal shape, unsigned count);

#ifdef __cplusplus
}
#endif

#define GEO_FRACTAL_H
#endif