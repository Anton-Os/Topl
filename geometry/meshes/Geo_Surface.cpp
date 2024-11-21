#include "Geo_Surface.hpp"

Geo_Surface::Geo_Surface(Shape2D shape, float z) : Geo_Mesh(shape.segments + 1, shape.segments * 3){
	_shape = shape;
	_depth = z;
	
    _vertices[0] = Geo_Vertex({ 0.0f, 0.0f, _depth }, { 0.5f, 0.5f, 0.0f }, { 0.0F, 0.0F, -1.0F }, { 1.0F, 1.0F, 1.0F }); // origin

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

    Geo_Mesh* targetMesh = new Geo_Mesh(*((Geo_Mesh*)this));
	for(unsigned i = 0; i < _iters; i++){
		unsigned short svCount = getVertexCount(); // start vertex count
    	unsigned short siCount = getIndexCount(); // start index count

		for(unsigned v = 0; v < svCount; v++){
			Geo_Mesh* newMesh = new Geo_Mesh(*targetMesh);
			newMesh->shift(_vertices[v].position);
			newMesh->scale({ 1.0F / (i + 2), 1.0F / (i + 2), 1.0F / (i + 2) });
			extend(*newMesh);
			delete newMesh;
		}

		for(unsigned i = 0; i < siCount; i++) _indices[i] = 0;
	}

	delete targetMesh;
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

    Geo_Mesh* targetMesh = new Geo_Mesh(*((Geo_Mesh*)this));
	for(unsigned i = 0; i < _iters; i++){
		unsigned short svCount = getVertexCount(); // start vertex count
    	unsigned short siCount = getIndexCount(); // start index count

		for(unsigned v = 0; v < svCount; v++){
			Geo_Mesh* newMesh = new Geo_Mesh(*targetMesh);
			newMesh->shift(_vertices[v].position);
			newMesh->scale({ 1.0F / (i + 2), 1.0F / (i + 2), 1.0F / (i + 2) });
			extend(*newMesh);
			delete newMesh;
		}

		for(unsigned i = 0; i < siCount; i++) _indices[i] = 0;
	}

	delete targetMesh;
}
