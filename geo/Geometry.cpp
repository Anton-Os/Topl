#include "Geometry.hpp"

// GeoVertex Operations

Geo_Vertex::Geo_Vertex(Eigen::Vector3f pos){
    position[X_OFFSET] = pos[X_OFFSET]; position[Y_OFFSET] = pos[Y_OFFSET]; position[Z_OFFSET] = pos[Z_OFFSET];
	texCoord[U_OFFSET] = 0.0f; texCoord[V_OFFSET] = 0.0f;
}

Geo_Vertex::Geo_Vertex(Eigen::Vector3f pos, Eigen::Vector2f texc){
    position[X_OFFSET] = pos[X_OFFSET]; position[Y_OFFSET] = pos[Y_OFFSET]; position[Z_OFFSET] = pos[Z_OFFSET];
		texCoord[U_OFFSET] = texc[U_OFFSET]; texCoord[V_OFFSET] = texc[V_OFFSET];
}

// Geo_RenderObj Operations

Geo_RenderObj::Geo_RenderObj(unsigned v){ // Vertex only constructor
    _verticesCount = v;
    if(_verticesCount % 2 == 0) _startAngle = TOPL_PI / (_verticesCount - 1); // offset angle for each even side length

    _posData = (Eigen::Vector3f*)malloc(_verticesCount * sizeof(Eigen::Vector3f));
    _normalsData = (Eigen::Vector3f*)malloc(_verticesCount * sizeof(Eigen::Vector3f));
    _texCoordData = (Eigen::Vector2f*)malloc(_verticesCount * sizeof(Eigen::Vector2f));
}

Geo_RenderObj::Geo_RenderObj(unsigned v, unsigned i){ // Vertex and Indices constructor
    _verticesCount = v;
    _indicesCount = i;
    if((_verticesCount - 1) % 2 == 0) _startAngle = TOPL_PI / (_verticesCount - 1); // offset angle for each even side length

    _posData = (Eigen::Vector3f*)malloc(_verticesCount * sizeof(Eigen::Vector3f));
    _normalsData = (Eigen::Vector3f*)malloc(_verticesCount * sizeof(Eigen::Vector3f));
    _texCoordData = (Eigen::Vector2f*)malloc(_verticesCount * sizeof(Eigen::Vector2f));
    if(_indicesCount != 0) _indices = (unsigned*)malloc(_indicesCount * sizeof(unsigned));
}

void Geo_RenderObj::cleanup() {
    if (_vertices != nullptr) free(_vertices);

    if (_posData != nullptr) free(_posData);
    if (_normalsData != nullptr) free(_normalsData);
    if (_texCoordData != nullptr) free(_texCoordData);
    if (_indices != nullptr) free(_indices);
} 

void Geo_RenderObj::modify(vTformCallback callback, double mod, AXIS_Target axis){
    if(_verticesCount == 0 || _posData == nullptr) return; // no processing can occur

    unsigned vAttributeOffset;
    switch(axis){
        case AXIS_X: vAttributeOffset = 0; break;
        case AXIS_Y: vAttributeOffset = 1; break;
        case AXIS_Z: vAttributeOffset = 2; break;
    }
    for(unsigned v = 0; v < _verticesCount; v++) // modify the position data of each vertex
        (*(_posData + v))[vAttributeOffset] = callback((*(_posData + v))[vAttributeOffset], mod); // updates specific vertex attribute
}

geoVertex_cptr Geo_RenderObj::getVertices(){
    if (_vertices == nullptr) {
        _vertices = (Geo_Vertex*)malloc(_verticesCount * sizeof(Geo_Vertex));
        for (unsigned v = 0; v < _verticesCount; v++)
            *(_vertices + v) = Geo_Vertex(*(_posData + v), *(_texCoordData + v)); // TODO: Include normals
    }
    return _vertices;
}

void Geo_RenderObj::fillRenderObject(){
    genVertices(_posData);
    genNormals(_normalsData);
    genTexCoords(_texCoordData);
    if(_indicesCount != 0) genIndices(_indices);
} 