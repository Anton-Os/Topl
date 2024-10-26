#include "Geo_Surface.hpp"

Geo_Surface::Geo_Surface(Shape2D shape, float z) : Geo_Mesh(shape.segments + 1, shape.segments * 3){
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

		// Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f });
        _vertices[v] = Geo_Vertex(pos);
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

Geo_Ext2D::Geo_Ext2D(Shape2D shape, float z, unsigned short iters) : Geo_Surface(shape, z){
	_iters = iters;
	
	unsigned short svCount = getVertexCount(); // start vertex count
	unsigned short siCount = getIndexCount(); // start index count
	
	for(unsigned l = 0; l < iters; l++){
		for(unsigned i = 0; i < siCount; i += 3){
			Geo_Vertex v1 = _vertices[_indices[i]];
       		Geo_Vertex v2 = _vertices[_indices[i + 1]];
       		Geo_Vertex v3 = _vertices[_indices[i + 2]];

			_vertices.push_back(Geo_Vertex(Vec3f(v1.position + v2.position + v3.position * (1.0 / 3.0))));
		}
		// Increment siCount for # of iterations?
	}

	for(unsigned i = 0; i < siCount; i += 3){ 
		// TODO: Perform loop for # of iterations?
		unsigned v = (svCount + i); // * ((iters - 1) * 3); // determine start vertex
		_indices.push_back(_indices[i]);
		_indices.push_back(_indices[i + 1]);
		_indices.push_back(v);

		_indices.push_back(_indices[i + 2]);
		_indices.push_back(_indices[i + 1]);
		_indices.push_back(v);

		_indices.push_back(_indices[i]);
		_indices.push_back(_indices[i + 2]);
		_indices.push_back(v);
	}
}

Geo_Surface::Geo_Surface(vertex_cptr_t points, unsigned short pointCount) : Geo_Mesh(pointCount, (pointCount - 2) * 3){
	unsigned short v = 0;
	for(unsigned p = 0; p < pointCount; p++){
		_vertices[v] = *(points + p);
        // _vertices[v].texcoord = getTexCoord(_vertices[v].position);
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

Geo_Ext2D::Geo_Ext2D(vertex_cptr_t points, unsigned short pointCount, unsigned short iters) : Geo_Surface(points, pointCount){
	_iters = iters;
	
	for(unsigned i = 0; i < iters; i++){
		for(unsigned v = 0; v < _vertices.size(); v++){
			// TODO: Perform vertex generation
		}

		if(i == iters - 1){
			// TODO: Perform indexing
		}
	}
}
