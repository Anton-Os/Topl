#include "Geometry.hpp"

/* static void genTexCoords_center(unsigned vertexCount, Vec2f* data){
    for (unsigned t = 0; t < vertexCount; t++)
        *(data + t) = Vec2f((Vec2f){ 0.5f, 0.5f}); // take the center value of texture
} */

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
    _vertexCount = v;
    if(_vertexCount % 2 == 0) _startAngle = TOPL_PI / (_vertexCount - 1); // offset angle for each even side length

    if(_vertexCount != 0){
        _pos = (Vec3f*)malloc(_vertexCount * sizeof(Vec3f));
        _normals = (Vec3f*)malloc(_vertexCount * sizeof(Vec3f));
        _texcoords = (Vec2f*)malloc(_vertexCount * sizeof(Vec2f));
    }
}

Geo_RenderObj::Geo_RenderObj(unsigned v, unsigned i){ // Vertex and Indices constructor
    _vertexCount = v;
    _indicesCount = i;
    if((_vertexCount - 1) % 2 == 0) _startAngle = TOPL_PI / (_vertexCount - 1); // offset angle for each even side length

    if(_vertexCount != 0){
        _pos = (Vec3f*)malloc(_vertexCount * sizeof(Vec3f));
        _normals = (Vec3f*)malloc(_vertexCount * sizeof(Vec3f));
        _texcoords = (Vec2f*)malloc(_vertexCount * sizeof(Vec2f));
    }
    if(_indicesCount != 0) _indices = (unsigned*)malloc(_indicesCount * sizeof(unsigned));
}

// Geo_RenderObj::Geo_RenderObj(const Geo_RenderObj& renderObj){ // copy constructor
void Geo_RenderObj::clone(const Geo_RenderObj* refObj){
    cleanup(); // make sure to completely erase old object first
    _vertexCount = refObj->getVertexCount();
    _indicesCount = refObj->getIndexCount();

    if(_vertexCount != 0){
        _pos = (Vec3f*)malloc(_vertexCount * sizeof(Vec3f));
        _normals = (Vec3f*)malloc(_vertexCount * sizeof(Vec3f));
        _texcoords = (Vec2f*)malloc(_vertexCount * sizeof(Vec2f));

        vec3f_cptr_t pos_ref = refObj->getPos();
        vec3f_cptr_t normals_ref = refObj->getNormals();
        vec2f_cptr_t texcoords_ref = refObj->getTexCoords();
        for(unsigned v = 0; v < _vertexCount; v++){
            *(_pos + v) = *(pos_ref + v); // copy position data
            *(_normals + v) = *(normals_ref + v); // copy normals data
            *(_texcoords + v) = *(texcoords_ref + v); // copy texcoord data
        }
    }

    if(_indicesCount != 0){ 
        _indices = (unsigned*)malloc(_indicesCount * sizeof(unsigned));

        ui_cptr_t indices_ref = refObj->getIndices();
        for(unsigned i = 0; i < _indicesCount; i++) *(_indices + i) = *(indices_ref + i);
    }
}

void Geo_RenderObj::fuse(const Geo_RenderObj* refObj){
    // TODO: backup current data here
    
    // cleanup(); // make sure to completely erase old object first
    _vertexCount += refObj->getVertexCount();
    _indicesCount += refObj->getIndexCount();
}

void Geo_RenderObj::modify(vTformCallback callback, double mod, AXIS_Target axis){
    if(_vertexCount == 0 || _pos == nullptr) return; // no processing can occur

    unsigned vAttribOffset;
    switch(axis){
        case AXIS_X: vAttribOffset = 0; break;
        case AXIS_Y: vAttribOffset = 1; break;
        case AXIS_Z: vAttribOffset = 2; break;
    }
	for (unsigned v = 0; v < _vertexCount; v++) {
		Vec3f* vertexPos = _pos + v;
		vertexPos->data[vAttribOffset] = callback(vertexPos->data[vAttribOffset], mod); // updates specific vertex attribute
	}
}

void Geo_RenderObj::fillRenderObj(){
    genPos(_pos);
    genNormals(_normals); // if(_normals == nullptr) genPos(_normals); // normals match position data if none are present
    genTexCoords(_texcoords); // if(_texcoords == nullptr) genTexCoords_center(getVertexCount(), _texcoords);
	// genVertices(); // generate vertices immediately
	if (_indicesCount != 0) genIndices(_indices);
}

void Geo_RenderObj::genVertices(){
    if(_vertices == nullptr) _vertices = (Geo_Vertex*)malloc(_vertexCount * sizeof(Geo_Vertex));
		
    for (unsigned v = 0; v < _vertexCount; v++)
        *(_vertices + v) = Geo_Vertex(*(_pos + v), *(_texcoords + v));
}

void Geo_RenderObj::cleanup() {
    _vertexCount = 0;
    _indicesCount = 0;

    if (_vertices != nullptr) free(_vertices);
    if (_pos != nullptr) free(_pos);
    if (_normals != nullptr) free(_normals);
    if (_texcoords != nullptr) free(_texcoords);
    _vertices = nullptr; _pos = nullptr; _normals = nullptr; _texcoords = nullptr;

    if (_indices != nullptr) free(_indices);
    _indices = nullptr;
} 