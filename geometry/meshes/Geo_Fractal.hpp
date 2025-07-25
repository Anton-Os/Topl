#ifndef GEO_FRACTAL_H

#include "Geo_Mesh.hpp"

struct ShapeFractal {
    float radius;
    float xDivs;
    float yDivs;
    float zDivs;
};

typedef bool (*fractalCallback)(Vec3f);

Geo_Vertex* genFractal_vertices(ShapeFractal shape, fractalCallback callback);
unsigned* genFractal_indices(Geo_Vertex* vertices, unsigned count);

#ifdef __cplusplus
class Geo_Fractal : public Geo_Mesh {
public:
    Geo_Fractal(ShapeFractal shape, fractalCallback callback){ _shape = shape; }
private:
    ShapeFractal _shape;
};
#endif

#define GEO_FRACTAL_H
#endif