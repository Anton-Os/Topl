#include "Geo_Fractal.hpp"

// Functions

bool fractalCull_ball(Vec3f input, double level) { return input.len() < level + 0.05 && input.len() > level - 0.05; }

TrigFractal fractalSpawn_trigs(Vec3f input){
    float f = FRACTAL_LEVEL * 0.0025;
    TrigFractal trig; // = TrigFractal({ 3, (Vec3f*)malloc(sizeof(Vec3f) * 3) });

    trig.pos1 = input;
    trig.pos2 = input + Vec3f({ sin(input.data[0] * 5) * f, cos(input.data[0] * 5) * f, cos(input.data[2] * 5) * f });
    trig.pos3 = input + Vec3f({ cos(input.data[1] * 5) * f, sin(input.data[1] * 5) * f, -sin(input.data[2] * 5) * f });

    return trig;
}

static Geo_Vertex* genFractal_lattice(ShapeFractal shape){
    unsigned count = shape.xDivs * shape.yDivs * shape.zDivs;
    Geo_Vertex* latticeVertices = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

    unsigned v = 0;
    for(float x = -shape.radius; x < shape.radius; x += ((shape.radius * 2.0f) / shape.xDivs))
        for(float y = -shape.radius; y < shape.radius; y += ((shape.radius * 2.0f) / shape.yDivs))
            for(float z = -shape.radius; z < shape.radius; z += ((shape.radius * 2.0f) / shape.zDivs)){
                if (v >= count) break; // assert(v < count); // TODO: Make sure the allocations are correct
                *(latticeVertices + v) = Geo_Vertex(Vec3f({ x, y, z }));
                v++;
            }

    return latticeVertices;
}

Geo_Vertex* genFractal_vertices(ShapeFractal shape, fCullCallback callback){
    unsigned count = shape.getCount();
    Geo_Vertex* latticeVertices = genFractal_lattice(shape);

    unsigned invalidCount = 0;
    for(unsigned v = 0; v < count; v++)
        if(!callback((*(latticeVertices + v)).position, FRACTAL_LEVEL)){
            *(latticeVertices + v) = Geo_Vertex(VEC_3F_INV); // invalid vertex
            invalidCount++;
        }

    Geo_Vertex* vertices = (Geo_Vertex*)malloc((count - invalidCount) * sizeof(Geo_Vertex) * 3);
    unsigned o = 0;
    for(unsigned v = 0; v < count; v++)
        if((*(latticeVertices + v)).position != VEC_3F_INV){
            TrigFractal trig = fractalSpawn_trigs((*(latticeVertices + v)).position);
            *(vertices + o) = Geo_Vertex(trig.pos1);
            *(vertices + o + 1) = Geo_Vertex(trig.pos2);
            *(vertices + o + 2) = Geo_Vertex(trig.pos3);
            o += 3;
        }

    free(latticeVertices);
    
    return vertices;
}

unsigned* genFractal_indices(ShapeFractal shape, unsigned count){
    unsigned* indices = (unsigned*)malloc(count * sizeof(unsigned));

    for(unsigned c = 0; c < count; c++) *(indices + c) = rand() % shape.getCount();

    return indices;
}

// Constructors

static unsigned getIdxCount(ShapeFractal shape, fCullCallback callback){
    Geo_Vertex* latticeVertices = genFractal_lattice(shape);

    unsigned invalidCount = 0;
    for(unsigned v = 0; v < shape.getCount(); v++)
        if(!callback((*(latticeVertices + v)).position, FRACTAL_LEVEL)){
            *(latticeVertices + v) = Geo_Vertex(VEC_3F_INV); // invalid vertex
            invalidCount++;
        }

    free(latticeVertices);

    return shape.getCount() - invalidCount;
}

Geo_Fractal::Geo_Fractal(ShapeFractal shape) : Geo_Mesh(
    getIdxCount(shape, fractalCull_ball), genFractal_vertices(shape, fractalCull_ball)
    // shape.getCount() / 3, genFractal_indices(shape, shape.getCount() / 3) // TODO: Improve this
) {
    _shape = shape;
}

Geo_Fractal::Geo_Fractal(ShapeFractal shape, fCullCallback callback) : Geo_Mesh(
    getIdxCount(shape, callback), genFractal_vertices(shape, callback)
    // shape.getCount() / 3, genFractal_indices(shape, shape.getCount() / 3) // TODO: Improve this
){
    _shape = shape;
}