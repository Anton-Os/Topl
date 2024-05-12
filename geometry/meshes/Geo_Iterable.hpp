#ifndef GEO_ITERATIVE_H

#include "Geo_Surface.hpp"
#include "Geo_Cone.hpp"
#include "Geo_Volume.hpp"

class Geo_Iter : public Geo_Mesh {
    Geo_Iter(const Geo_Mesh* mesh, unsigned short i) : Geo_Mesh(mesh->getVertices() * i, mesh->getIndices() * i){
        refMesh = mesh;
    }

    // TODO: Include body

    const Geo_Mesh* refMesh;
};

class Geo_Tess : public Geo_Iter { Geo_Tess(Geo_Mesh* mesh); }; // Breaks shape into subsections

class Geo_Duplex : public Geo_Iter { Geo_Duplex(Geo_Mesh* mesh); }; // Expands shape into supersets


#endif