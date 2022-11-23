#include "Geo_Conic.hpp"

void Geo_Conic::genVertices() {
	_vertices[0] = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z + Z_INCREMENT }, { 0.5f, 0.5f }); // origin
	_vertices[_vertices.size() - 1] = Geo_Vertex(_apex, { 0.5f, 0.5f }); // apex

	for (unsigned v = 1; v < _vertices.size() - 1; v++) {
		Vec3f pos = Vec3f({ 
			(float)sin(_shape.getInitAngle() + (v * _shape.getAngle())) * _shape.getSize(),
			(float)cos(_shape.getInitAngle() + (v * _shape.getAngle())) * _shape.getSize(),
			(float)DEFAULT_Z
		});

		Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f }); // base facing normal

		Vec2f texcoord;
		switch ((v - 1) % 4) {
			case 0: texcoord = Vec2f({ 1.0f, 0.0f }); break; // bottom left
			case 1: texcoord = Vec2f({ 1.0f, 1.0f }); break; // top right
			case 2: texcoord = Vec2f({ 0.0f, 1.0f }); break; // bottom right
			case 3: texcoord = Vec2f({ 0.0f, 0.0f }); break; // top left
		}

		_vertices[v] = Geo_Vertex(pos, texcoord);
	}
}

void Geo_Conic::genIndices() {
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