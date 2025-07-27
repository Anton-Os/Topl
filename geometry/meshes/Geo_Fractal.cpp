#include "Geo_Fractal.hpp"

// Functions

bool fractalBall(Vec3f input) { return input.len() < 0.55F && input.len() > 0.45F; } // include all points

Geo_Vertex* genFractal_vertices(ShapeFractal shape, fractalCallback callback){
    unsigned count = shape.xDivs * shape.yDivs * shape.zDivs;
    Geo_Vertex* all_vertices = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

    unsigned v = 0;
    for(float x = -shape.radius; x < shape.radius; x += ((shape.radius * 2.0f) / shape.xDivs))
        for(float y = -shape.radius; y < shape.radius; y += ((shape.radius * 2.0f) / shape.yDivs))
            for(float z = -shape.radius; z < shape.radius; z += ((shape.radius * 2.0f) / shape.zDivs)){
                if (v >= count) break; // assert(v < count); // TODO: Make sure the allocations are correct
                *(all_vertices + v) = Geo_Vertex(Vec3f({ x, y, z }));
                v++;
            }

    unsigned invalidCount = 0;
    for(v = 0; v < count; v++)
        if(!callback((*(all_vertices + v)).position)){
            *(all_vertices + v) = Geo_Vertex(VEC_3F_INV); // invalid vertex
            invalidCount++;
        }

    Geo_Vertex* vertices = (Geo_Vertex*)malloc((count - invalidCount) * sizeof(Geo_Vertex));
    unsigned o = 0;
    for(v = 0; v < count; v++)
        if((*(all_vertices + v)).position != VEC_3F_INV){
            *(vertices + o) = *(all_vertices + v);
            o++;
        }

    free(all_vertices);
    
    return vertices;
}

unsigned* genFractal_indices(ShapeFractal shape, unsigned count){
    unsigned* indices = (unsigned*)malloc(count * sizeof(unsigned));

    for(unsigned c = 0; c < count; c++) *(indices + c) = rand() % shape.getCount();

    return indices;
}

// Constructors

static unsigned getFractalIdxCount(ShapeFractal shape, fractalCallback callback){
    unsigned count = shape.xDivs * shape.yDivs * shape.zDivs;
    Geo_Vertex* all_vertices = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

    unsigned v = 0;
    for(float x = -shape.radius; x < shape.radius; x += ((shape.radius * 2.0f) / shape.xDivs))
        for(float y = -shape.radius; y < shape.radius; y += ((shape.radius * 2.0f) / shape.yDivs))
            for(float z = -shape.radius; z < shape.radius; z += ((shape.radius * 2.0f) / shape.zDivs)){
                if (v >= count) break; // assert(v < count); // TODO: Make sure the allocations are correct
                *(all_vertices + v) = Geo_Vertex(Vec3f({ x, y, z }));
                v++;
            }

    unsigned invalidCount = 0;
    for(v = 0; v < count; v++)
        if(!callback((*(all_vertices + v)).position)){
            *(all_vertices + v) = Geo_Vertex(VEC_3F_INV); // invalid vertex
            invalidCount++;
        }

    free(all_vertices);

    return count - invalidCount;
}

Geo_Fractal::Geo_Fractal(ShapeFractal shape) : Geo_Mesh(
    getFractalIdxCount(shape, fractalBall), genFractal_vertices(shape, fractalBall)
    // shape.getCount() / 3, genFractal_indices(shape, shape.getCount() / 3) // TODO: Improve this
) {
    _shape = shape;
}

Geo_Fractal::Geo_Fractal(ShapeFractal shape, fractalCallback callback) : Geo_Mesh(
    getFractalIdxCount(shape, callback), genFractal_vertices(shape, callback)
    // shape.getCount() / 3, genFractal_indices(shape, shape.getCount() / 3) // TODO: Improve this
){
    _shape = shape;
}