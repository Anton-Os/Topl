#ifndef GEO_ITERATIVE_H

#include "Geo_Surface.hpp"
#include "Geo_Cone.hpp"
#include "Geo_Volume.hpp"

class Geo_Iter : public Geo_Mesh {
public:
    Geo_Iter(const Geo_Mesh* mesh, unsigned short vCount, unsigned short iCount) : Geo_Mesh(vCount, iCount){
        refMesh = mesh;
    }
    Geo_Iter(const Geo_Mesh* mesh, unsigned short iters) : Geo_Mesh(mesh->getVertexCount() + ((mesh->getIndexCount()) * iters), mesh->getIndexCount() * 3 * iters){
        refMesh = mesh;
    }

    // TODO: Include body

protected:
    const Geo_Mesh* refMesh;
};

struct Geo_TessIter : public Geo_Iter { // Breaks shape into subsections
    Geo_TessIter(const Geo_Mesh* mesh, unsigned short iters); 
};

struct Geo_DuplexIter : public Geo_Iter { // Expands shape into supersets
    Geo_DuplexIter(const Geo_Mesh* mesh); // copies geometry at every vertex
    // Geo_DuplexIter(const Geo_Surface* surface, unsigned short iters); // duplex geometry creating grid based on edges
    // Geo_DuplexIter(const Geo_Cone* cone, Vec3f direction); // duplex geometry creating a hub based on direction
    // Geo_DuplexIter(const Geo_Volume* volume, std::initializer_list<Vec3f> pathSet); // duplex geometry creating a sequence based on path series 
}; 


#endif