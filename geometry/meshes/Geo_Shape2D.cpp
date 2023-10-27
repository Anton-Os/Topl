#include "Geo_Shape2D.hpp"

void Geo_Shape::genVertices() {
	_vertices[0] = Geo_Vertex({ 0.0f, 0.0f, _depth }, { 0.5f, 0.5f, 0.0f }); // origin

	for (unsigned v = 1; v < _vertices.size(); v++) {
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(_shape.segments) + ((v - 1) * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)cos(ANGLE_START(_shape.segments) + ((v - 1) * ANGLE_OFFSET(_shape.segments))) * RADIUS_SIZE(_shape.radius),
			(float)_depth
		});

		Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f });
		Vec3f texcoord = getTexCoord(v, 0.0f);

		_vertices[v] = Geo_Vertex(pos, texcoord);
	}
}

void Geo_Shape::genIndices() {
	unsigned v = 1; // tracks current vertex
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