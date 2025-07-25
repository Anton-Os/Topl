#ifndef GEO_FRACTAL_H

#include "Geo_Mesh.hpp"

struct ShapeFractal {
    unsigned getCount() { return xDivs * yDivs * zDivs; }

    float radius;
    unsigned xDivs;
    unsigned yDivs;
    unsigned zDivs;
};

typedef bool (*fractalCallback)(Vec3f);

bool fractalAll(Vec3f input);

Geo_Vertex* genFractal_vertices(ShapeFractal shape, fractalCallback callback);
unsigned* genFractal_indices(Geo_Vertex* vertices, unsigned count);

#ifdef __cplusplus
class Geo_Fractal : public Geo_Mesh {
public:
    Geo_Fractal(ShapeFractal shape);
    Geo_Fractal(ShapeFractal shape, fractalCallback callback);
private:
    ShapeFractal _shape;
};
#endif

#define GEO_FRACTAL_H
#endif