#include "Geo_Freeform.hpp"

// Culling Functions

bool freeformCull_none(Vec3f input, double level) { return true; } // no culling
bool freeformCull_ball(Vec3f input, double level) { return input.len() < level + 0.05 && input.len() > level - 0.05; }

// Spawning Functions

TrigForm freeformSpawn_test(Vec3f input, unsigned count) {
    return TrigForm({ input, input * FREEFORM_LEVEL, input * FREEFORM_LEVEL * FREEFORM_LEVEL });
}

Geo_Meshlet freeformSpawn_lines(Vec3f input, unsigned count) {
    Geo_Meshlet lineMeshlet;
    lineMeshlet.count = 2;
    lineMeshlet.positions = (Vec3f*)malloc(sizeof(Vec3f) * 2);
    for (unsigned p = 0; p < 2; p++) *(lineMeshlet.positions + p) = input * ((p == 0)? 1.0F : (p * FREEFORM_LEVEL));
    return lineMeshlet;
}

// Generation Functions

static Geo_Vertex* genFreeform_lattice(ShapeFreeform shape, fCullCallback cullCallback){
    unsigned count = shape.xDivs * shape.yDivs * shape.zDivs;
    Geo_Vertex* latticeVertices = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

    unsigned v = 0, b = 0; // tracks total vertices and invalid
    for(float x = -shape.radius * 0.5F; x <= shape.radius * 0.5F; x += shape.radius / shape.xDivs)
        for(float y = -shape.radius * 0.5F; y <= shape.radius * 0.5F; y += shape.radius / shape.yDivs)
            for(float z = -shape.radius * 0.5F; z <= shape.radius * 0.5F; z += shape.radius / shape.zDivs){
                if (v >= count) break; // assert(v < count); // TODO: Make sure the allocations are correct
                
                if (cullCallback((*(latticeVertices + v)).position, FREEFORM_LEVEL)) 
                    *(latticeVertices + v) = Geo_Vertex(Vec3f({ x, y, z })); // regular lattice
                else {
                    *(latticeVertices + v) = VEC_3F_BAD;
                    b++; // bad vertex increment
                }
                v++;
            }

    return latticeVertices;
}

// Geo_Vertex* genFreeform_vertices(ShapeFreeform shape, fSpawnCallback spawnCallback, fCullCallback cullCallback){
Geo_Vertex* genFreeform_vertices(ShapeFreeform shape, fSpawnCallback spawnCallback, fCullCallback cullCallback) {
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
    // Geo_Vertex* vertices = (Geo_Vertex*)malloc((count - invalidVertCount) * sizeof(Geo_Vertex) * 3);
    std::vector<Geo_Vertex> vertices;
    // unsigned o = 0;
    for(unsigned v = 0; v < count; v++)
        if((*(latticeVertices + v)).position != VEC_3F_BAD){
            // TrigForm trig = spawnCallback((*(latticeVertices + v)).position, count - invalidVertCount);
            // for (unsigned t = 0; t < 3; t++) *(vertices + o + t) = trig.positions[t];
            // o += 3;
            Geo_Meshlet meshlet = spawnCallback((*(latticeVertices + v)).position, count - invalidVertCount);
            for (unsigned c = 0; c < meshlet.count; c++) vertices.push_back(Geo_Vertex(*(meshlet.positions + c)));
            free(meshlet.positions); // make sure to deallocate memory
        }

    free(latticeVertices);
    
    Geo_Vertex* vertexData = (Geo_Vertex*)malloc(sizeof(Geo_Vertex) * vertices.size());
    std::copy(vertices.begin(), vertices.end(), vertexData);
    return vertexData;
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
    getIdxCount(shape, freeformCull_none), genFreeform_vertices(shape, freeformSpawn_lines, freeformCull_none)
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
