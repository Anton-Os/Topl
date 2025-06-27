#include "Geo_Volume.hpp"

// Functions

Geo_Vertex* genVolume_vertices(Shape2D shape, float d){
	unsigned count = (shape.segments + 1) * 2;
	Geo_Vertex* vertexData = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));
	
	*(vertexData) = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z + (d / 2) }, { 0.5, 0.5, 0.0f }, { 0.0F, 0.0F, -1.0F }, { 1.0F, 1.0F, 1.0F });

	unsigned v = 1;
	for (v = 1; v < count / 2; v++) { // front face vertices
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(shape.segments) + (v * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)cos(ANGLE_START(shape.segments) + (v * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)DEFAULT_Z + (d / 2)
		});

		*(vertexData + v) = Geo_Vertex(pos);
	}

    *(vertexData + (count / 2)) = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z - (d / 2) }, { 0.5, 0.5, 1.0f }, { 0.0F, 0.0F, 1.0F }, { 1.0F, 1.0F, 1.0F });
	for (v = 1 + (count / 2); v < count; v++) { // back face vertices
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(shape.segments) + ((v - (count / 2)) * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)cos(ANGLE_START(shape.segments) + ((v - (count / 2)) * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)DEFAULT_Z - (d / 2)
		});

		*(vertexData + v) = Geo_Vertex(pos);
	}

	return vertexData;
}

unsigned* genVolume_indices(Shape2D shape){
	unsigned vCount = (shape.segments + 1) * 2;
	unsigned iCount = shape.segments * 12;
	unsigned* indexData = (unsigned*)malloc(iCount * sizeof(unsigned));

	unsigned i; // current index
	unsigned v = 1; // tracks current vertex

	// Front Face Indexing
	for (i = 0; i < (iCount / 4) - 3; i += 3) {
		*(indexData + i) = 0; // origin
		*(indexData + i + 1) = v; // target
		*(indexData + i + 2) = v + 1; // next vertex
		v++;
	}

	*(indexData + i) = 0;
	*(indexData + i + 1) = v;
	*(indexData + i + 2) = 1;

	// Back Face Indexing
	v = (vCount / 2) + 1;
	for (i = iCount / 4; i < (iCount / 2) - 3; i += 3) {
		*(indexData + i) = vCount / 2; // origin
		*(indexData + i + 1) = v; // target
		*(indexData + i + 2) = v + 1; // next vertex
		v++;
	}

	*(indexData + i) = vCount / 2;
	*(indexData + i + 1) = v;
	*(indexData + i + 2) = (vCount / 2) + 1;

	// Side Face Indexing
	v = 1;
	for (i = iCount / 2; i < iCount - 6; i += 6) {
		*(indexData + i) = v;
		*(indexData + i + 1) = v + 1;
		*(indexData + i + 2) = v + (vCount / 2) + 1;
		*(indexData + i + 3) = v + (vCount / 2);
		*(indexData + i + 4) = v;
		*(indexData + i + 5) = v + (vCount / 2) + 1;
		v++;
	}

	*(indexData + i) = 1;
	*(indexData + i + 1) = (vCount / 2) + 1;
	*(indexData + i + 2) = (vCount / 2) - 1;
	*(indexData + i + 3) = vCount - 1;
	*(indexData + i + 4) = (vCount / 2) - 1;
	*(indexData + i + 5) = (vCount / 2) + 1;

	return indexData;
}

// Constructors

Geo_Volume::Geo_Volume(Shape2D shape, float depth) : Geo_Mesh((shape.segments + 1) * 2, shape.segments * 12) {
	_shape = shape;
	_depth = depth;
	
	unsigned v;

    _vertices[0] = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z + (_depth / 2) }, { 0.5, 0.5, 0.0f }, { 0.0F, 0.0F, -1.0F }, { 1.0F, 1.0F, 1.0F });
	for (v = 1; v < _vertices.size() / 2; v++) { // front face vertices
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)cos(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)DEFAULT_Z + (_depth / 2)
		});

		// Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f }); // front face normal
		_vertices[v] = Geo_Vertex(pos);
	}

    _vertices[_vertices.size() / 2] = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z - (_depth / 2) }, { 0.5, 0.5, 1.0f }, { 0.0F, 0.0F, 1.0F }, { 1.0F, 1.0F, 1.0F });
	for (v = 1 + (_vertices.size() / 2); v < _vertices.size(); v++) { // back face vertices
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(_shape.segments) + ((v - (_vertices.size() / 2)) * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)cos(ANGLE_START(_shape.segments) + ((v - (_vertices.size() / 2)) * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)DEFAULT_Z - (_depth / 2)
		});

		// Vec3f normal = Vec3f({ 0.0f, 0.0f, 1.0f }); // back face normal
		_vertices[v] = Geo_Vertex(pos);
	}

	unsigned i; // current index
	v = 1; // tracks current vertex

	{
		// Front Face Indexing
		for (i = 0; i < (_indices.size() / 4) - 3; i += 3) {
			_indices[i] = 0; // origin
			_indices[i + 1] = v; // target
			_indices[i + 2] = v + 1; // next vertex
			v++;
		}

		// special case for last front triangle
		_indices[i] = 0;
		_indices[i + 1] = v;
		_indices[i + 2] = 1;
	}

	{
		// Back Face Indexing
		v = (_vertices.size() / 2) + 1;
		for (i = _indices.size() / 4; i < (_indices.size() / 2) - 3; i += 3) {
			_indices[i] = _vertices.size() / 2; // origin
			_indices[i + 1] = v; // target
			_indices[i + 2] = v + 1; // next vertex
			v++;
		}

		// special case for last back triangle
		_indices[i] = _vertices.size() / 2;
		_indices[i + 1] = v;
		_indices[i + 2] = (_vertices.size() / 2) + 1;
	}

	{
		// Side Face Indexing
		v = 1;
		for (i = _indices.size() / 2; i < _indices.size() - 6; i += 6) {
			_indices[i] = v;
			_indices[i + 1] = v + 1;
			_indices[i + 2] = v + (_vertices.size() / 2) + 1;
			_indices[i + 3] = v + (_vertices.size() / 2);
			_indices[i + 4] = v;
			_indices[i + 5] = v + (_vertices.size() / 2) + 1;
			v++;
		}

		// special cases for sides
		_indices[i] = 1;
		_indices[i + 1] = (_vertices.size() / 2) + 1;
		_indices[i + 2] = (_vertices.size() / 2) - 1;
		_indices[i + 3] = _vertices.size() - 1;
		_indices[i + 4] = (_vertices.size() / 2) - 1;
		_indices[i + 5] = (_vertices.size() / 2) + 1;
	}
}

Geo_Ext3D::Geo_Ext3D(Shape2D shape, float depth, unsigned short iters) : Geo_Volume(shape, depth){
	_iters = iters;

	Geo_Mesh* targetMesh = new Geo_Mesh(*((Geo_Mesh*)this));
	for(unsigned i = 0; i < _iters; i++){
		Geo_Mesh* newMesh = new Geo_Mesh(*targetMesh);
		newMesh->shift({ 0.0F, 0.0F, (i + 1) * depth });
		extend(*newMesh);
		delete newMesh;
	}

	delete targetMesh;
}

Geo_Volume::Geo_Volume(vertex_cptr_t points, unsigned short pointCount, float depth) : Geo_Mesh(pointCount * 2, (pointCount * 3) * 3){
	_depth = depth;

	unsigned short v = 0;
	for(unsigned p = 0; p < pointCount; p++){
		_vertices[v] = *(points + p);
		_vertices[v].position.data[2] += _depth / 2;
        // _vertices[v].texcoord = getTexCoord(_vertices[v].position);
		_vertices[_vertices.size() - 1 - v] = *(points + p);
		_vertices[_vertices.size() - 1 - v].position.data[2] -= _depth / 2;
		// _vertices[_vertices.size() - 1 - v].texcoord = getTexCoord(_vertices[_vertices.size() - 1 - v].position);
        v++; 
	}

	v = 0; // reset vertex
	unsigned short i = 0;
	while(i < _indices.size() / 6){ // indexing front face
		if(i % 2 == 0){
			_indices[i] = v;
			_indices[i + 1] = v + 1;
			_indices[i + 2] = v + 2;
		} else {
			_indices[i] = v;
			_indices[i + 1] = v + 2;
			_indices[i + 2] = v + 3;
			v += 3;
		}

		i += 3;
	}

	v = 0; // reset vertex
	while(i < (_indices.size() / 6) * 2){ // indexing back face
		if(i % 2 == 0){
			_indices[i] = _vertices.size() - 3 - v;
			_indices[i + 1] = _vertices.size() - 2 - v;
			_indices[i + 2] = _vertices.size() - 1 - v;
		} else {
			_indices[i] = _vertices.size() - 1 - v;
			_indices[i + 1] = _vertices.size() - 4 - v;
			_indices[i + 2] = _vertices.size() - 3 - v;
			v += 3;
		}

		i += 3;
	}

	v = 0; // reset vertex
	while(i < _indices.size()){ // indexing sides
		_indices[i] = v;
		_indices[i + 1] = _vertices.size() - 1 - v;
		_indices[i + 2] = v + 1;
		_indices[i + 3] = _vertices.size() - 1 - v;
		_indices[i + 4] = _vertices.size() - 2 - v;
		_indices[i + 5] = v + 1;
		v++;
		i += 6;
	}
}

Geo_Ext3D::Geo_Ext3D(vertex_cptr_t points, unsigned short pointCount, float depth, unsigned short iters) : Geo_Volume(points, pointCount, depth){
	_iters = iters;

	Geo_Mesh* targetMesh = new Geo_Mesh(*((Geo_Mesh*)this));
	for(unsigned i = 0; i < _iters; i++){
		Geo_Mesh* newMesh = new Geo_Mesh(*targetMesh);
		newMesh->shift({ 0.0F, 0.0F, (i + 1) * depth });
		extend(*newMesh);
		delete newMesh;
	}

	delete targetMesh;
}
