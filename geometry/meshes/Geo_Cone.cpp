#include "Geo_Cone.hpp"

Geo_Cone::Geo_Cone(Shape shape, Vec3f apex) : Geo_Mesh(shape.segments + 2, shape.segments * 6) {
	_shape = shape;
	_apex = apex;

	_vertices[0] = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z + 0.00001F }, { 0.5f, 0.5f, 0.0f }); // origin
	_vertices[_vertices.size() - 1] = Geo_Vertex(_apex, { 0.5f, 0.5f, 1.0f }); // apex

	unsigned v;
	for (v = 1; v < _vertices.size() - 1; v++) {
		Vec3f pos = Vec3f({ 
			(float)sin(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)cos(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)DEFAULT_Z
		});

		Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f }); // base facing normal
		Vec3f texcoord = getTexCoord(pos);

		_vertices[v] = Geo_Vertex(pos, texcoord);
	}

	unsigned i; // current index
	v = 1; // tracks current vertex

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

Geo_ExtCone::Geo_ExtCone(Shape shape, Vec3f apex, unsigned short iters) : Geo_Cone(shape, apex){
	_iters = iters;

	for(unsigned i = 0; i < iters; i++){
		for(unsigned s = 0; s < shape.segments; s++){
			// TODO: Perform vertex generation
			// TODO: Perform indexing
		}
	}
}

Geo_Cone::Geo_Cone(Vec3f* points, unsigned short pointCount, Vec3f apex) : Geo_Mesh(pointCount + 1, (pointCount + 2) * 3){
	_apex = apex;
	_vertices[_vertices.size() - 1] = Geo_Vertex(_apex, { 0.5f, 0.5f, 1.0f }); // apex
	
	unsigned short v = 0;
	for(unsigned p = 0; p < pointCount; p++){
		_vertices[v] = *(points + p);
        _vertices[v].texcoord = getTexCoord(_vertices[v].position);
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
		v++;
	}

	// TODO: Indexing connecting to apex
}

Geo_ExtCone::Geo_ExtCone(Vec3f* points, unsigned short pointCount, Vec3f apex, unsigned short iters) : Geo_Cone(points, pointCount, apex){
	_iters = iters;

	unsigned short svCount = getVertexCount(); // start vertex count
	unsigned short siCount = getIndexCount(); // start index count

	for(unsigned l = 0; l < iters; l++)
		for(unsigned p = 0; p < pointCount; p++){
			Geo_Vertex newVertex = _vertices[p];
			newVertex.position.data[0] *= 1.1; // for testing
			newVertex.position.data[1] *= 1.1; // for testing
			newVertex.position.data[2] += apex.data[2] * (l + 1);
			_vertices.push_back(newVertex);

			// TODO: Index face?

			unsigned v = p + svCount;
			for(unsigned i = _indices.size(); i < _indices.size() + (pointCount * 6); i += 6){ // indexing sides
				_indices.push_back(v);
				_indices.push_back(_vertices.size() - 1 - v);
				_indices.push_back(v + 1);
				_indices.push_back(_vertices.size() - 1 - v);
				_indices.push_back(_vertices.size() - 2 - v);
				_indices.push_back(v + 1);
				v++;
			}
		}
}