#include "Geo_Sphere.hpp"

void Geo_Sphere::genVertices() {
	unsigned v = 0; // target vertex

	for (unsigned stack = 0; stack <= _shape.xSegments; stack++) {
		float stackAngle = (MATH_PI / 2) - (stack * (MATH_PI / _shape.xSegments));
		float xy = _shape.getSize() * cosf(stackAngle);
		float z = _shape.getSize() * sinf(stackAngle);
		for (unsigned slice = 0; slice <= _shape.ySegments; slice++) {
			// float sliceAngle = slice * ((2 * MATH_PI) / _shape.ySegments);
			float sliceAngle = slice * _shape.getAngleY();
			float x = xy * cosf(sliceAngle);
			float y = xy * sinf(sliceAngle);

			Vec3f pos = Vec3f({ x, y, z });
			Vec3f normal = Vec3f({ x, y, z });
			Vec2f texcoord = Vec2f({ x, y });

			_vertices[v] = Geo_Vertex(pos, texcoord);
			v++; // increment to next vertex
		}
	}
}

void Geo_Sphere::genIndices() {
	unsigned i = 0;

	for (unsigned stack = 0; stack <= _shape.xSegments; stack++) {
		unsigned stackIndex = stack * (_shape.ySegments + 1);
		unsigned nextIndex = stackIndex + _shape.ySegments + 1;
		for (unsigned slice = 0; slice <= _shape.ySegments; slice++) {
			stackIndex++;
			nextIndex++;

			if (stack != 0) { // index all except first stack
				_indices[i] = stackIndex;
				_indices[i + 1] = nextIndex;
				_indices[i + 2] = stackIndex + 1;
				i += 3;
			}
			if (stack < _shape.xSegments - 1) { // index all except last stack
				_indices[i] = stackIndex + 1;
				_indices[i + 1] = nextIndex;
				_indices[i + 2] = nextIndex + 1;
				i += 3;
			}
		}
	}
}