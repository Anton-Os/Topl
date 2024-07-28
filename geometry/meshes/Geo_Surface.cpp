#include "Geo_Surface.hpp"

Geo_Surface::Geo_Surface(Shape shape, float z) : Geo_Mesh(shape.segments + 1, shape.segments * 3){
	_shape = shape;
	_depth = z;
	
	_vertices[0] = Geo_Vertex({ 0.0f, 0.0f, _depth }, { 0.5f, 0.5f, 0.0f }); // origin

	unsigned v;
	for (v = 1; v < _vertices.size(); v++) {
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(_shape.segments) + ((v - 1) * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)cos(ANGLE_START(_shape.segments) + ((v - 1) * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)_depth
		});

		Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f });
		Vec3f texcoord = getTexCoord(v, 0.0f);
		// Vec3f texcoord = getTexCoord(_vertices[v].position);

		_vertices[v] = Geo_Vertex(pos, texcoord);
	}

	v = 1; // tracks current vertex
	for (unsigned i = 0; i < _indices.size() - 3; i += 3) {
		_indices[i + 0] = 0; // origin
		_indices[i + 1] = v; // target
		_indices[i + 2] = v + 1; // next vertex
		v++;
	}

	// special case for last triangle
	_indices[_indices.size() - 3] = 0;
	_indices[_indices.size() - 2] = v;
	_indices[_indices.size() - 1] = 1;
}

Geo_Surface::Geo_Surface(Vec3f* points, unsigned short pointCount) : Geo_Mesh(pointCount, (pointCount - 2) * 3){
	unsigned short v = 0;
	for(unsigned p = 0; p < pointCount; p++){
		_vertices[v] = *(points + p);
        _vertices[v].texcoord = _vertices[v].position;
        v++; 
	}

	v = 0; // reset vertex
	unsigned short i = 0;
	while(i < _indices.size()){ // indexing for surface
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
}