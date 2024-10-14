#include "Geo_Volume.hpp"

void Geo_Volume::genVertices() {
	_vertices[0] = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z + (_depth / 2) }, { 0.5, 0.5, 0.0f });
	for (unsigned v = 1; v < _vertices.size() / 2; v++) { // front face vertices
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)cos(ANGLE_START(_shape.segments) + (v * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)DEFAULT_Z + (_depth / 2)
		});

		// Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f }); // front face normal
		Vec3f texcoord = getTexCoord(v, 0.0f);

		_vertices[v] = Geo_Vertex(pos, texcoord);
	}

	_vertices[_vertices.size() / 2] = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z - (_depth / 2) }, { 0.5, 0.5, 1.0f });
	for (unsigned v = 1 + (_vertices.size() / 2); v < _vertices.size(); v++) { // back face vertices
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(_shape.segments) + ((v - (_vertices.size() / 2)) * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)cos(ANGLE_START(_shape.segments) + ((v - (_vertices.size() / 2)) * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)DEFAULT_Z - (_depth / 2)
		});

		// Vec3f normal = Vec3f({ 0.0f, 0.0f, 1.0f }); // back face normal
		Vec3f texcoord = getTexCoord(v, 1.0f);

		_vertices[v] = Geo_Vertex(pos, texcoord);
	}
}

void Geo_Volume::genIndices() {
	unsigned i; // current index
	unsigned v = 1; // tracks current vertex

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