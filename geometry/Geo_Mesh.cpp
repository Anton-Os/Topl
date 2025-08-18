#include "Geo_Mesh.hpp"

// Helper Functions

void free_vertices(Geo_Vertex* vertices){ 
    free(vertices); 
    vertices = NULL;
}

void free_indices(unsigned* indices){ 
    free(indices); 
    indices = NULL;
}

// Shape Objects

Shape2D create_shape2D(float radius, unsigned short segs){ return Shape2D({ radius, segs }); }

Shape3D create_shape3D(float radius, unsigned short xSegs, unsigned short ySegs){ return Shape3D({ radius, xSegs, ySegs }); }

Geo_Vertex* tesselate(Geo_Vertex* vertices, unsigned vertexCount, unsigned short tessCount){
    Geo_Vertex* tessVertices = (Geo_Vertex*)malloc(vertexCount * sizeof(Geo_Vertex));
    for(unsigned v = 0; v < vertexCount; v++) *(tessVertices + v) = *(vertices + v); // copying operation

    /* if(tessCount == 0) return;
    unsigned short svCount, siCount;

    for(unsigned l = 0; l < tessCount; l++){
        svCount = getVertexCount(); // start vertex count
        siCount = getIndexCount(); // start index count

        if(siCount > 0){
            for(unsigned i = 0; i < siCount; i += 3) // determining tesselated vertices
                _vertices.push_back(Geo_Vertex(Vec3f((_vertices[_indices[i]].position + _vertices[_indices[i + 1]].position + _vertices[_indices[i + 2]].position) * (1.0 / 3.0))));
                // _vertices.push_back(getMidpoint({ _vertices[_indices[i]], _vertices[_indices[i + 1]], _vertices[_indices[i + 2]] })); // TODO: Get this working!

            for(unsigned i = 0; i < siCount; i += 3){
                unsigned v = (svCount + (i / 3));
                _indices.push_back(_indices[i]);
                _indices.push_back(_indices[i + 1]);
                _indices.push_back(v); // first triangle
                _indices.push_back(_indices[i + 2]);
                _indices.push_back(_indices[i + 1]);
                _indices.push_back(v); // second triangle
                _indices.push_back(_indices[i]);
                _indices.push_back(_indices[i + 2]);
                _indices.push_back(v); // third triangle
            }
            for (unsigned i = 0; i < siCount; i++) _indices.erase(_indices.begin());
        } else {
            for(unsigned v = 0; v < svCount; v += 3){
                Geo_Vertex midpoint = Geo_Vertex(Vec3f((_vertices[v + 0].position + _vertices[v + 1].position + _vertices[v + 2].position) * (1.0 / 3.0)));
                _vertices.push_back(midpoint); _vertices.push_back(_vertices[v]); _vertices.push_back(_vertices[v + 1]); // 1st triangle
                _vertices.push_back(midpoint); _vertices.push_back(_vertices[v]); _vertices.push_back(_vertices[v + 2]); // 2nd triangle
                _vertices.push_back(midpoint); _vertices.push_back(_vertices[v + 1]); _vertices.push_back(_vertices[v + 2]); // 3rd triangle
            }
            for (unsigned v = 0; v < svCount; v++) _vertices.erase(_vertices.begin());
        }

        drawMax = (siCount > 0)? _indices.size() : _vertices.size();
    }

    _tessLevel += tessCount; // for testing */

    return tessVertices;
}

// Mesh Object

Geo_Mesh::Geo_Mesh(vertex_cptr_t points, unsigned short pointCount){ // point set constructor
    _vertices.resize(pointCount); 
    _indices.resize(pointCount);

    for(auto p = 0; p < pointCount; p++){
        _vertices[p] = *(points + p);
        _indices[p] = p; // choose indexing scheme?
    }
}
	
Geo_Mesh::Geo_Mesh(const Geo_Mesh& refMesh){
    for(unsigned short v = 0; v < refMesh.getVertexCount(); v++) _vertices.push_back(*(refMesh.getVertices() + v));
    for(unsigned short i = 0; i < refMesh.getIndexCount(); i++) _indices.push_back(*(refMesh.getIndices() + i));
    // TODO: Copy instance and other data?
}

void Geo_Mesh::modify(vTformCallback callback, Vec3f transform) { // modify position attirbute
    for (std::vector<Geo_Vertex>::iterator v = _vertices.begin(); v != _vertices.end(); v++)
        *(&v->position) = callback(v->position, transform);
    _transformCount++;
}

void Geo_Mesh::modify(iTformCallback callback, Vec3f transform){
    for (std::vector<unsigned>::iterator i = _indices.begin(); i != _indices.end(); i++)
        _vertices[*i] = callback(*i, _vertices[*i].position, transform);
    _transformCount++;
}

void Geo_Mesh::modify(vertexTransform callback){
    for (unsigned v = 0; v < _vertices.size(); v++)
        _vertices[v] = callback(_vertices[v], v / 3, _transformCount);
    _transformCount++;
}

void Geo_Mesh::modify(originTransform callback){
    bool isIndexed = getIndexCount() > 0;
    for (unsigned m = 0; m < ((isIndexed)? _indices.size() : _vertices.size()) / 3; m++){
        if(m + 2 > _indices.size() && _indices.size() > 0) break;
        unsigned idx1 = (isIndexed)? _indices[(m * 3) + 0] : (m * 3) + 0;
        unsigned idx2 = (isIndexed)? _indices[(m * 3) + 1] : (m * 3) + 1;
        unsigned idx3 = (isIndexed)? _indices[(m * 3) + 2] : (m * 3) + 2;
        Geo_Vertex midpoint = Geo_Vertex((_vertices[idx1].position + _vertices[idx2].position + _vertices[idx3].position) * (1.0f / 3.0f));
        // Geo_Vertex midpoint = getMidpoint({ _vertices[idx1], _vertices[idx2], _vertices[idx3] }); // TODO: Get this working!
        _vertices[idx1] = callback(_vertices[idx1], midpoint, m, _transformCount);
        _vertices[idx2] = callback(_vertices[idx2], midpoint, m, _transformCount);
        _vertices[idx3] = callback(_vertices[idx3], midpoint, m, _transformCount);
    }
    _transformCount++;
}

void Geo_Mesh::extend(const Geo_Mesh& refMesh){
    unsigned svCount = getVertexCount();
    
    for(unsigned v = 0; v < refMesh.getVertexCount(); v++)
        _vertices.push_back(*(refMesh.getVertices() + v));

    for(unsigned i = 0; i < refMesh.getIndexCount(); i++)
        _indices.push_back(*(refMesh.getIndices() + i) + svCount);
}

void Geo_Mesh::tesselate(unsigned short tessCount){ // TODO: Include Tesselation for Non-Indexed Mesh
    if(tessCount == 0) return;
    unsigned short svCount, siCount;

    for(unsigned l = 0; l < tessCount; l++){
        svCount = getVertexCount(); // start vertex count
        siCount = getIndexCount(); // start index count

        if(siCount > 0){
            for(unsigned i = 0; i < siCount; i += 3) // determining tesselated vertices
                _vertices.push_back(Geo_Vertex(Vec3f((_vertices[_indices[i]].position + _vertices[_indices[i + 1]].position + _vertices[_indices[i + 2]].position) * (1.0 / 3.0))));
                // _vertices.push_back(getMidpoint({ _vertices[_indices[i]], _vertices[_indices[i + 1]], _vertices[_indices[i + 2]] })); // TODO: Get this working!

            for(unsigned i = 0; i < siCount; i += 3){
                unsigned v = (svCount + (i / 3));
                _indices.push_back(_indices[i]);
                _indices.push_back(_indices[i + 1]);
                _indices.push_back(v); // first triangle
                _indices.push_back(_indices[i + 2]);
                _indices.push_back(_indices[i + 1]);
                _indices.push_back(v); // second triangle
                _indices.push_back(_indices[i]);
                _indices.push_back(_indices[i + 2]);
                _indices.push_back(v); // third triangle
            }
            for (unsigned i = 0; i < siCount; i++) _indices.erase(_indices.begin());
        } else {
            for(unsigned v = 0; v < svCount; v += 3){
                Geo_Vertex midpoint = Geo_Vertex(Vec3f((_vertices[v + 0].position + _vertices[v + 1].position + _vertices[v + 2].position) * (1.0 / 3.0)));
                _vertices.push_back(midpoint); _vertices.push_back(_vertices[v]); _vertices.push_back(_vertices[v + 1]); // 1st triangle
                _vertices.push_back(midpoint); _vertices.push_back(_vertices[v]); _vertices.push_back(_vertices[v + 2]); // 2nd triangle
                _vertices.push_back(midpoint); _vertices.push_back(_vertices[v + 1]); _vertices.push_back(_vertices[v + 2]); // 3rd triangle
            }
            for (unsigned v = 0; v < svCount; v++) _vertices.erase(_vertices.begin());
        }

        drawMax = (siCount > 0)? _indices.size() : _vertices.size();
    }

    _tessLevel += tessCount; // for testing
}

void Geo_Mesh::setInstances(std::initializer_list<Mat4x4> matrices){
    _instanceCount = matrices.size();
    // if (_instanceData != nullptr) free(_instanceData);
    if (_instanceData == nullptr) { // testing
        _instanceData = (Mat4x4*)malloc(matrices.size() * sizeof(Mat4x4));

        unsigned instanceOffset = 0;
        for (auto m = matrices.begin(); m != matrices.end() && instanceOffset < MAX_INSTANCES; m++) {
            *(_instanceData + instanceOffset) = *m;
            instanceOffset++;
        }
    }
}

std::string Geo_Mesh::toString() const { // creates a printed version of a mesh
    std:: string meshStr = "Vertices: {";
    for(unsigned v = 0; v < getVertexCount(); v++) meshStr += _vertices[v].position.toString() + ", ";
    meshStr += (getIndexCount() > 0)? "}\nIndices: {" : "}";
    if(getIndexCount() > 0){
        for(unsigned i = 0; i < getIndexCount(); i++) meshStr += std::to_string(i) + ", ";
        meshStr += "}";
    }
    return meshStr;
}

void Geo_Mesh::populate(unsigned vertexCount, Geo_Vertex* vertexData, unsigned indexCount, unsigned* indexData){
    if(vertexCount > 0 && vertexData != nullptr){
        _vertices.resize(vertexCount);
        for(unsigned v = 0; v < vertexCount; v++) _vertices[v] = *(vertexData + v);
        free_vertices(vertexData);
    }
    if(indexCount > 0 && indexData != nullptr){
        _indices.resize(indexCount);
        for(unsigned i = 0; i < indexCount; i++) _indices[i] = *(indexData + i);
        free(indexData);
    } else {
        _indices.resize(vertexCount);
        for (unsigned i = 0; i < vertexCount; i++) _indices[i] = i;
    }
}
