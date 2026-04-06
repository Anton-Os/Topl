#include "Geo_Freeform.hpp"

// Culling Functions

bool freeformCull_none(Vec3f input, double level) { return true; } // no culling
bool freeformCull_ball(Vec3f input, double level) { return input.len() < level + 0.05 && input.len() > level - 0.05; }

// Spawning Functions

TrigForm freeformSpawn_test(Vec3f input, unsigned count){
    static unsigned v = 0;
    float f = FREEFORM_LEVEL * 0.25F;
    v == (count == 0)? 0 : v + 1;

    /* return TrigForm({
        input, 
        input + Vec3f({ sin(input.data[0] * 5) * f, cos(input.data[0] * 3) * f, cos(input.data[2] * 10) * f}),
        input - Vec3f({ cos(input.data[0] * 3) * f, sin(input.data[1] * 5) * f, -sin(input.data[2] * 10) * f})
    }); */

    return TrigForm({
        input,
        input + Vec3f({ sin(input.len() * 5) * f, cos(input.len() * 3) * f, cos(input.len() * 10) * f}),
        input - Vec3f({ cos(input.len() * 3) * f, sin(input.len() * 5) * f, -sin(input.len() * 10) * f})
    });

    /* return TrigForm({
        input,
        input + Vec3f({ input.data[0] + (v % 2 == 0)? 0.0F : f, input.data[2] + (v % 3 == 0)? f : 0.0F, input.data[1] + (v % 4 == 0)? -f : f }),
        input - Vec3f({ input.data[1] + (v % 5 == 0)? f : 0.0F, input.data[0] + (v % 6 == 0)? 0.0F : f, input.data[1] + (v % 7 == 0)? -f : f })
    }); */
}

// Generation Functions

static Geo_Vertex* genFreeform_lattice(ShapeFreeform shape, fCullCallback cullCallback){
    unsigned count = shape.xDivs * shape.yDivs * shape.zDivs;
    Geo_Vertex* latticeVertices = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

    unsigned v = 0, b = 0; // tracks total vertices and invalid
    for(float x = -shape.radius; x < shape.radius; x += ((shape.radius * 2.0f) / shape.xDivs))
        for(float y = -shape.radius; y < shape.radius; y += ((shape.radius * 2.0f) / shape.yDivs))
            for(float z = -shape.radius; z < shape.radius; z += ((shape.radius * 2.0f) / shape.zDivs)){
                if (v >= count) break; // assert(v < count); // TODO: Make sure the allocations are correct
                
                if (cullCallback((*(latticeVertices + v)).position, FREEFORM_LEVEL)) *(latticeVertices + v) = Geo_Vertex(Vec3f({ x, y, z })); // regular lattice
                else {
                    *(latticeVertices + v) = VEC_3F_BAD;
                    b++; // bad vertex increment
                }
                v++;
            }

    return latticeVertices;
}

Geo_Vertex* genFreeform_vertices(ShapeFreeform shape, fSpawnCallback spawnCallback, fCullCallback cullCallback){
    unsigned count = shape.getCount();
    Geo_Vertex* latticeVertices = genFreeform_lattice(shape, cullCallback);

    // Culling
    unsigned invalidVertCount = 0;
    for(unsigned v = 0; v < count; v++)
        if(!cullCallback((*(latticeVertices + v)).position, FREEFORM_LEVEL)){
            *(latticeVertices + v) = Geo_Vertex(VEC_3F_BAD); // invalid vertex
            invalidVertCount++;
        }

    // Spawning
    Geo_Vertex* vertices = (Geo_Vertex*)malloc((count - invalidVertCount) * sizeof(Geo_Vertex) * 3);
    unsigned o = 0;
    for(unsigned v = 0; v < count; v++)
        if((*(latticeVertices + v)).position != VEC_3F_BAD){
            TrigForm trig = spawnCallback((*(latticeVertices + v)).position, count - invalidVertCount);
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
    Geo_Vertex* latticeVertices = genFreeform_lattice(shape, callback);

    unsigned invalidCount = 0;
    for(unsigned v = 0; v < shape.getCount(); v++)
        if(!callback((*(latticeVertices + v)).position, FREEFORM_LEVEL)){
            *(latticeVertices + v) = Geo_Vertex(VEC_3F_BAD); // invalid vertex
            invalidCount++;
        }

    free(latticeVertices);

    return shape.getCount() - invalidCount;
}

// Constructors

Geo_Freeform::Geo_Freeform(ShapeFreeform shape) : Geo_Mesh(
    getIdxCount(shape, freeformCull_none), genFreeform_vertices(shape, freeformSpawn_test, freeformCull_none)
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
