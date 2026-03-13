#include "Geo_Freeform.hpp"

// Functions

bool freeformCull_ball(Vec3f input, double level) { return input.len() < level + 0.05 && input.len() > level - 0.05; }

TrigForm freeformSpawn_trigs(Vec3f input){
    float f = FREEFORM_LEVEL * 0.0025;
    TrigForm trig; // = TrigForm({ 3, (Vec3f*)malloc(sizeof(Vec3f) * 3) });

    trig.pos1 = input;
    trig.pos2 = input + Vec3f({ sin(input.data[0] * 5) * f, cos(input.data[0] * 5) * f, cos(input.data[2] * 5) * f });
    trig.pos3 = input + Vec3f({ cos(input.data[1] * 5) * f, sin(input.data[1] * 5) * f, -sin(input.data[2] * 5) * f });

    return trig;
}

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

Geo_Vertex* genFreeform_vertices(ShapeFreeform shape, fCullCallback callback){
    unsigned count = shape.getCount();
    Geo_Vertex* latticeVertices = genFreeform_lattice(shape);

    unsigned invalidCount = 0;
    for(unsigned v = 0; v < count; v++)
        if(!callback((*(latticeVertices + v)).position, FREEFORM_LEVEL)){
            *(latticeVertices + v) = Geo_Vertex(VEC_3F_INV); // invalid vertex
            invalidCount++;
        }

    Geo_Vertex* vertices = (Geo_Vertex*)malloc((count - invalidCount) * sizeof(Geo_Vertex) * 3);
    unsigned o = 0;
    for(unsigned v = 0; v < count; v++)
        if((*(latticeVertices + v)).position != VEC_3F_INV){
            TrigForm trig = freeformSpawn_trigs((*(latticeVertices + v)).position);
            *(vertices + o) = Geo_Vertex(trig.pos1);
            *(vertices + o + 1) = Geo_Vertex(trig.pos2);
            *(vertices + o + 2) = Geo_Vertex(trig.pos3);
            o += 3;
        }

    free(latticeVertices);
    
    return vertices;
}

unsigned* genFreeform_indices(ShapeFreeform shape, unsigned count){
    unsigned* indices = (unsigned*)malloc(count * sizeof(unsigned));

    for(unsigned c = 0; c < count; c++) *(indices + c) = rand() % shape.getCount();

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
    getIdxCount(shape, freeformCull_ball), genFreeform_vertices(shape, freeformCull_ball)
    // shape.getCount() / 3, genFreeform_indices(shape, shape.getCount() / 3) // TODO: Improve this
) {
    _shape = shape;
}

Geo_Freeform::Geo_Freeform(ShapeFreeform shape, fCullCallback callback) : Geo_Mesh(
    getIdxCount(shape, callback), genFreeform_vertices(shape, callback)
    // shape.getCount() / 3, genFreeform_indices(shape, shape.getCount() / 3) // TODO: Improve this
){
    _shape = shape;
}