#include "Geo_Iterable.hpp"

Geo_TessIter::Geo_TessIter(const Geo_Mesh* mesh, unsigned short iters) : Geo_Iter(mesh, iters){
    // TODO: Do this recursively

    unsigned v = 0;
    unsigned t = 0;
    for(v = 0; v < mesh->getVertexCount(); v++) _vertices[v] = *(mesh->getVertices() + v);
    while(v < _vertices.size()){
       Geo_Vertex v1 = *(mesh->getVertices() + (*(mesh->getIndices() + t) % mesh->getIndexCount()));
       Geo_Vertex v2 = *(mesh->getVertices() + (*(mesh->getIndices() + t + 1) % mesh->getIndexCount()));
       Geo_Vertex v3 = *(mesh->getVertices() + (*(mesh->getIndices() + t + 2) % mesh->getIndexCount()));

        _vertices[v] = Geo_Vertex(
            Vec3f(v1.position + v2.position + v3.position * (1.0 / 3.0)), 
            Vec3f(v1.texcoord + v2.texcoord + v3.texcoord * (1.0 / 3.0))
        );
       
        v++;
        t+= 3;
    }
    
    unsigned i = 0;
    for(i = 0; i < mesh->getIndexCount(); i++) _indices[i] = *(mesh->getIndices() + i); // TODO: Replace with real indices
    while(i < _indices.size()){
        // _indices[i] = 0; // TODO: Replace with real indices
        _indices[i] = mesh->getVertexCount() + i;
        _indices[i + 1] = mesh->getVertexCount() + i + 1;
        _indices[i + 2] = mesh->getVertexCount() + i + 2;
        i += 3;
    }
}

Geo_DuplexIter::Geo_DuplexIter(const Geo_Mesh* mesh) : Geo_Iter(mesh, mesh->getVertexCount() * mesh->getVertexCount(), mesh->getIndexCount() * mesh->getVertexCount()){
    unsigned v = 0;
    unsigned i = 0;
    for(unsigned g = 0; g < mesh->getVertexCount(); g++){
        Geo_Vertex refVertex = *(mesh->getVertices() + g);

        for(unsigned l = 0; l < mesh->getVertexCount(); l++){
            _vertices[v] = Geo_Vertex(// refVertex + *(mesh->getVertices() + g);
                Vec3f(refVertex.position + (mesh->getVertices() + l)->position),
                Vec3f(refVertex.texcoord + (mesh->getVertices() + l)->texcoord)
            );
            v++;
        }

        for(unsigned o = 0; o < mesh->getIndexCount(); o++){
            _indices[i] = *(mesh->getIndices() + (o * g));
            i++;
        }
    }
}

/* Geo_DuplexIter::Geo_DuplexIter(const Geo_Mesh* mesh) : Geo_Iter(mesh, 1){
    unsigned v = 0;
    for(v = 0; v < mesh->getVertexCount(); v++) _vertices[v] = *(mesh->getVertices() + v);
    while(v < _vertices.size()){
        _vertices[v] = Geo_Vertex({ 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}); // TODO: Replace with real vertices
        v++;
    }
    
    unsigned i = 0;
    for(i = 0; i < mesh->getIndexCount(); i++) _indices[i] = *(mesh->getIndices() + i); // TODO: Replace with real indices
    while(i < _indices.size()){
        _indices[i] = 0; // TODO: Replace with real indices
        i++;
    }
} */
