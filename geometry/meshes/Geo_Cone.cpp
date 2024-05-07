#include "Geo_Cone.hpp"

Geo_Cone::Geo_Cone(Shape shape) : Geo_Mesh(shape.segments + 2, shape.segments * 6) {
	_shape = shape;
	_apex = Vec3f({ 0.0f, 0.0f, shape.radius + DEFAULT_Z }); // default apex
	genVertices();
	genIndices();
}

Geo_Cone::Geo_Cone(Shape shape, Vec3f apex) : Geo_Mesh(shape.segments + 2, shape.segments * 6) {
	_shape = shape;
	_apex = apex;
	genVertices(); 
	genIndices();
}

Geo_Cone::Geo_Cone(std::initializer_list<Vec3f> pointsSet, Vec3f apex) : Geo_Mesh(pointsSet.size() + 1, (pointsSet.size() + 2) * 3){
	_apex = apex;
	_vertices[_vertices.size() - 1] = Geo_Vertex(_apex, { 0.5f, 0.5f, 1.0f }); // apex
	
	unsigned short v = 0;
	for(auto p = pointsSet.begin(); p != pointsSet.end(); p++){
		_vertices[v] = *p;
        _vertices[v].texcoord = _vertices[v].position;
        v++; 
	}

	v = 0; // reset vertex
	unsigned short i = 0;
	while(i < _indices.size() / 3){ // indexing for face
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
	while(i < _indices.size()){
		_indices[i] = _vertices.size() - 1; // connect to apex
		_indices[i + 1] = v;
		_indices[i + 2] = v + 1;
		i += 3;
	}

	// TODO: Indexing connecting to apex
}

void Geo_Cone::genVertices() {
	_vertices[0] = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z + 0.00001F }, { 0.5f, 0.5f, 0.0f }); // origin
	_vertices[_vertices.size() - 1] = Geo_Vertex(_apex, { 0.5f, 0.5f, 1.0f }); // apex

	for (unsigned v = 1; v < _vertices.size() - 1; v++) {
		Vec3f pos = Vec3f({ 
			(float)sin(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)cos(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)DEFAULT_Z
		});

		Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f }); // base facing normal
		Vec3f texcoord = getTexCoord(v, 0.0f);

		_vertices[v] = Geo_Vertex(pos, texcoord);
	}
}

void Geo_Cone::genIndices() {
	unsigned i; // current index
	unsigned v = 1; // tracks current vertex

	{
		// Base Indexing
		for (i = 0; i < (_indices.size() / 2) - 3; i += 3) {
			_indices[i] = 0; // origin
			_indices[i + 1] = v; // target
			_indices[i + 2] = v + 1; // next vertex
			v++;
		}

		// special case for last base triangle
		_indices[i] = 0;
		_indices[i + 1] = v;
		_indices[i + 2] = 1;
	}

	{
		// Apex Indexing
		v = 1; // reset

		for (i = _indices.size() / 2; i < _indices.size() - 3; i += 3) {
			_indices[i] = _vertices.size() - 1; // apex
			_indices[i + 1] = v + 1; // next vertex
			_indices[i + 2] = v; // target
			v++;
		}

		// special case for last apex triangle
		_indices[i] = _vertices.size() - 1;
		_indices[i + 1] = v;
		_indices[i + 2] = 1;
	}
}