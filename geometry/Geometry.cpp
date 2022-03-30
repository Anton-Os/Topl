#include "Geometry.hpp"

// Geo_Vertex Operations

Geo_Vertex::Geo_Vertex(Vec3f pos){
    position[0] = pos[0]; position[1] = pos[1]; position[2] = pos[2];
	texcoord[0] = 0.0f; texcoord[1] = 0.0f;
}

Geo_Vertex::Geo_Vertex(Vec3f pos, Vec2f texc){
    position[0] = pos[0]; position[1] = pos[1]; position[2] = pos[2];
	texcoord[0] = texc[0]; texcoord[1] = texc[1];
}

// Geo_RenderObj Operations

Geo_RenderObj::Geo_RenderObj(unsigned v){ // Vertex only constructor
    _verticesCount = v;
    if(_verticesCount % 2 == 0) _startAngle = TOPL_PI / (_verticesCount - 1); // offset angle for each even side length

    if(_verticesCount != 0){
        _posData = (Vec3f*)malloc(_verticesCount * sizeof(Vec3f));
        _normalsData = (Vec3f*)malloc(_verticesCount * sizeof(Vec3f));
        _texcoordData = (Vec2f*)malloc(_verticesCount * sizeof(Vec2f));
    }
}

Geo_RenderObj::Geo_RenderObj(unsigned v, unsigned i){ // Vertex and Indices constructor
    _verticesCount = v;
    _indicesCount = i;
    if((_verticesCount - 1) % 2 == 0) _startAngle = TOPL_PI / (_verticesCount - 1); // offset angle for each even side length

    if(_verticesCount != 0){
        _posData = (Vec3f*)malloc(_verticesCount * sizeof(Vec3f));
        _normalsData = (Vec3f*)malloc(_verticesCount * sizeof(Vec3f));
        _texcoordData = (Vec2f*)malloc(_verticesCount * sizeof(Vec2f));
    }
    if(_indicesCount != 0) _indices = (unsigned*)malloc(_indicesCount * sizeof(unsigned));
}

// Geo_RenderObj::Geo_RenderObj(const Geo_RenderObj& renderObj){ // copy constructor
void Geo_RenderObj::clone(const Geo_RenderObj* refObj){
    cleanup(); // make sure to completely erase old object first
    _verticesCount = refObj->getVerticesCount();
    _indicesCount = refObj->getIndexCount();

    if(_verticesCount != 0){
        _posData = (Vec3f*)malloc(_verticesCount * sizeof(Vec3f));
        _normalsData = (Vec3f*)malloc(_verticesCount * sizeof(Vec3f));
        _texcoordData = (Vec2f*)malloc(_verticesCount * sizeof(Vec2f));

        vec3f_cptr_t posData_ref = refObj->getPosData();
        vec3f_cptr_t normalsData_ref = refObj->getNormalsData();
        vec2f_cptr_t texcoordData_ref = refObj->getTexCoordData();
        for(unsigned v = 0; v < _verticesCount; v++){
            *(_posData + v) = *(posData_ref + v); // copy position data
            *(_normalsData + v) = *(normalsData_ref + v); // copy normals data
            *(_texcoordData + v) = *(texcoordData_ref + v); // copy texcoord data
        }
    }

    if(_indicesCount != 0){ 
        _indices = (unsigned*)malloc(_indicesCount * sizeof(unsigned));

        ui_cptr_t indices_ref = refObj->getIndices();
        for(unsigned i = 0; i < _indicesCount; i++) *(_indices + i) = *(indices_ref + i);
    }
}

void Geo_RenderObj::cleanup() {
    _verticesCount = 0;
    _indicesCount = 0;

    if (_vertices != nullptr) free(_vertices);
    if (_posData != nullptr) free(_posData);
    if (_normalsData != nullptr) free(_normalsData);
    if (_texcoordData != nullptr) free(_texcoordData);
    _vertices = nullptr; _posData = nullptr; _normalsData = nullptr; _texcoordData = nullptr;

    if (_indices != nullptr) free(_indices);
    _indices = nullptr;
} 

void Geo_RenderObj::modify(vTformCallback callback, double mod, AXIS_Target axis){
    if(_verticesCount == 0 || _posData == nullptr) return; // no processing can occur

    unsigned vAttribOffset;
    switch(axis){
        case AXIS_X: vAttribOffset = 0; break;
        case AXIS_Y: vAttribOffset = 1; break;
        case AXIS_Z: vAttribOffset = 2; break;
    }
	for (unsigned v = 0; v < _verticesCount; v++) {
		Vec3f* vertexPos = _posData + v;
		vertexPos->data[vAttribOffset] = callback(vertexPos->data[vAttribOffset], mod); // updates specific vertex attribute
	}
}

void Geo_RenderObj::fillRenderObj(){
    genPos(_posData);
    genNormals(_normalsData);
    genTexCoords(_texcoordData);

	// Generate Vertices Immediately
	// genVertices();

	if (_indicesCount != 0) genIndices(_indices);
}

void Geo_RenderObj::genVertices(){
    if(_vertices == nullptr) _vertices = (Geo_Vertex*)malloc(_verticesCount * sizeof(Geo_Vertex));
		
    for (unsigned v = 0; v < _verticesCount; v++) // TODO: Make regeneration configurable!
        *(_vertices + v) = Geo_Vertex(*(_posData + v), *(_texcoordData + v));
}