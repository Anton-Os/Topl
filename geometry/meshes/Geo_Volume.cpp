#include "Geo_Volume.hpp"

Geo_Volume::Geo_Volume(Shape2D shape, float depth) : Geo_Mesh((shape.segments + 1) * 2, shape.segments * 12) {
	_shape = shape;
	_depth = depth;
	
	unsigned v;

	_vertices[0] = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z + (_depth / 2) }, { 0.5, 0.5, 0.0f });
	for (v = 1; v < _vertices.size() / 2; v++) { // front face vertices
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)cos(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)DEFAULT_Z + (_depth / 2)
		});

		// Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f }); // front face normal
		_vertices[v] = Geo_Vertex(pos);
	}

	_vertices[_vertices.size() / 2] = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z - (_depth / 2) }, { 0.5, 0.5, 1.0f });
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

	unsigned short svCount = getVertexCount(); // start vertex count
	unsigned short siCount = getIndexCount(); // start index count

	for(unsigned l = 0; l < iters; l++)
		for(unsigned p = 0; p < shape.segments; p++){
            /* Geo_Vertex newVertex = _vertices[p];
			newVertex.position.data[2] += depth * (l + 1);
			_vertices.push_back(newVertex);

			// TODO: Index face?

			unsigned v = p + svCount;
			for(unsigned i = siCount; i < siCount + (shape.segments * 6); i += 6){ // indexing sides
				_indices.push_back(v);
				_indices.push_back(_vertices.size() - 1 - v);
				_indices.push_back(v + 1);
				_indices.push_back(_vertices.size() - 1 - v);
				_indices.push_back(_vertices.size() - 2 - v);
				_indices.push_back(v + 1);
				v++;
            } */
		}
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

	unsigned short svCount = getVertexCount(); // start vertex count
	unsigned short siCount = getIndexCount(); // start index count

	for(unsigned l = 0; l < iters; l++)
		for(unsigned p = 0; p < pointCount; p++){
			Geo_Vertex newVertex = _vertices[p];
			newVertex.position.data[2] += depth * (l + 1);
			_vertices.push_back(newVertex);

			// TODO: Index face?

			unsigned v = p + svCount;
			for(unsigned i = siCount; i < siCount + (pointCount * 6); i += 6){ // indexing sides
				_indices.push_back(v);
                _indices.push_back(svCount - 1 - v);
				_indices.push_back(v + 1);
                _indices.push_back(svCount - 1 - v);
                _indices.push_back(svCount - 2 - v);
				_indices.push_back(v + 1);
				v++;

				// printf("Volume vertex is %d with size %d, index is %d with size %d", v, _vertices.size(), i, _indices.size());
			}
		}
}
