#include "Geo_Freeform.hpp"

// Culling Functions

bool freeformCull_none(Vec3f input, double level) { return true; } // no culling
bool freeformCull_ball(Vec3f input, double level) { return input.len() < level + 0.05 && input.len() > level - 0.05; }

// Spawning Functions

TrigForm freeformSpawn_trigs(Vec3f input){
    float f = FREEFORM_LEVEL * 0.25F;
    return TrigForm({ 
        input, 
        input + Vec3f({ sin(input.data[0] * 5) * f, cos(input.data[0] * 3) * f, cos(input.data[2] * 10) * f}),
        input - Vec3f({ cos(input.data[1] * 3) * f, sin(input.data[1] * 5) * f, -sin(input.data[2] * 10) * f})
    });
}

// Generation Functions

static Geo_Vertex* genFreeform_lattice(ShapeFreeform shape){
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

Geo_Vertex* genFreeform_vertices(ShapeFreeform shape, fSpawnCallback spawnCallback, fCullCallback cullCallback){
    unsigned count = shape.getCount();
    Geo_Vertex* latticeVertices = genFreeform_lattice(shape);

    // Culling
    unsigned invalidVertCount = 0;
    for(unsigned v = 0; v < count; v++)
        if(!cullCallback((*(latticeVertices + v)).position, FREEFORM_LEVEL)){
            *(latticeVertices + v) = Geo_Vertex(VEC_3F_INV); // invalid vertex
            invalidVertCount++;
        }

    // Spawning
    Geo_Vertex* vertices = (Geo_Vertex*)malloc((count - invalidVertCount) * sizeof(Geo_Vertex) * 3);
    unsigned o = 0;
    for(unsigned v = 0; v < count; v++)
        if((*(latticeVertices + v)).position != VEC_3F_INV){
            TrigForm trig = spawnCallback((*(latticeVertices + v)).position);
            for (unsigned t = 0; t < 3; t++) *(vertices + o + t) = trig.positions[t];
            o += 3;
        }

    free(latticeVertices);
    
    return vertices;
}

unsigned* genFreeform_indices(ShapeFreeform shape, unsigned count){
    unsigned* indices = (unsigned*)malloc(count * sizeof(unsigned));

    for(unsigned c = 0; c < count; c++) *(indices + c) = rand() % shape.getCount(); // TODO: Replace this

    return indices;
}

static unsigned getIdxCount(ShapeFreeform shape, fCullCallback callback){
    Geo_Vertex* latticeVertices = genFreeform_lattice(shape);

    unsigned invalidCount = 0;
    for(unsigned v = 0; v < shape.getCount(); v++)
        if(!callback((*(latticeVertices + v)).position, FREEFORM_LEVEL)){
            *(latticeVertices + v) = Geo_Vertex(VEC_3F_INV); // invalid vertex
            invalidCount++;
        }

    free(latticeVertices);

    return shape.getCount() - invalidCount;
}

// Constructors

Geo_Freeform::Geo_Freeform(ShapeFreeform shape) : Geo_Mesh(
    getIdxCount(shape, freeformCull_none), genFreeform_vertices(shape, freeformSpawn_trigs, freeformCull_ball)
    // shape.getCount() / 3, genFreeform_indices(shape, shape.getCount() / 3) // TODO: Improve this
) {
    _shape = shape;
}

Geo_Freeform::Geo_Freeform(ShapeFreeform shape, fSpawnCallback spawnCallback, fCullCallback cullCallback) : Geo_Mesh(
    getIdxCount(shape, cullCallback), genFreeform_vertices(shape, spawnCallback, cullCallback)
    // shape.getCount() / 3, genFreeform_indices(shape, shape.getCount() / 3) // TODO: Improve this
){
    _shape = shape;
}