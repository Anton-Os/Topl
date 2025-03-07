#include "Geo_Orboid.hpp"

void Geo_Orb::init() {
	unsigned v = 0; // target vertex

	for (unsigned stack = 0; stack <= _shape.xSegs; stack++) {
		float stackAngle = (MATH_PI / 2) - (stack * (MATH_PI / _shape.xSegs));
		float xy = RADIUS_SIZE(_shape.radius) * cosf(stackAngle);
		float z = RADIUS_SIZE(_shape.radius) * sinf(stackAngle);
		for (unsigned slice = 0; slice <= _shape.ySegs; slice++) {
			// float sliceAngle = slice * ((2 * MATH_PI) / _shape.ySegments);
			float sliceAngle = slice * ANGLE_OFFSET(_shape.ySegs);
			float x = xy * cosf(sliceAngle);
			float y = xy * sinf(sliceAngle);

			Vec3f pos = Vec3f({ x, y, z });
			_vertices[v] = Geo_Vertex(pos);
			v++; // increment to next vertex
		}
	}

	unsigned i = 0;

	for (unsigned stack = 0; stack <= _shape.xSegs; stack++) {
		unsigned stackIndex = stack * (_shape.ySegs + 1);
		unsigned nextIndex = stackIndex + _shape.ySegs + 1;
		for (unsigned slice = 0; slice <= _shape.ySegs; slice++) {
			stackIndex++;
			nextIndex++;

			_indices[i] = stackIndex;
			_indices[i + 1] = nextIndex;
			_indices[i + 2] = stackIndex + 1; // (stackIndex + 1) % _shape.xSegs;
			i += 3;

			if (stack < _shape.xSegs - 1) { // index all except last stack
				_indices[i] = stackIndex + 1; // (stackIndex + 1) % _shape.xSegs;
				_indices[i + 1] = nextIndex;
				_indices[i + 2] = nextIndex + 1; // (nextIndex + 1) % _shape.xSegs;
				i += 3;
			} else {
				// missing triangle
				_indices[i] = _shape.xSegs;
				_indices[i + 1] = _shape.xSegs + 1;
				_indices[i + 2] = _shape.xSegs + 2;
			}
		}
	}
}

void Geo_Torus::init(){
	unsigned v = 0;

	float x, y, z, xy;
	float sectorStep = (2 * MATH_PI) / _shape.xSegs;
	float sideStep = (2 * MATH_PI) / _shape.ySegs;

	for(unsigned innerRing = 0; innerRing <= _shape.xSegs; innerRing++){
		float sideAngle = MATH_PI - (innerRing * sideStep);
		xy = (_shape.radius * _diameter) * cosf(sideAngle);
		z = (_shape.radius * _diameter) * sinf(sideAngle);

		for(unsigned outerRing = 0; outerRing < _shape.ySegs; outerRing++){
			float sectorAngle = outerRing * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);

			x += _shape.radius * cosf(sectorAngle);
			y += _shape.radius * sinf(sectorAngle);

			_vertices[v] = Geo_Vertex({ x, y, z });
			v++;
		}
	}

	unsigned i = 0;

	for(unsigned innerRing = 0; innerRing <= _shape.xSegs; innerRing++){
		unsigned idx = innerRing * (_shape.ySegs + 1);
		unsigned nextIdx = idx + _shape.ySegs + 1;

		for(unsigned outerRing = 0; outerRing < _shape.ySegs + 1; outerRing++){
			_indices[i + 0] = idx;
			_indices[i + 1] = nextIdx;
			_indices[i + 2] = idx + 1;
			_indices[i + 3] = idx + 1;
			_indices[i + 4] = nextIdx;
			_indices[i + 5] = nextIdx + 1;
			i += 6;
			idx++; nextIdx++;
		}
	}

	// printf("Vertex count is %d, vertex total is %d, Index count is %d, Index total is %d", v, getVertexCount(), i, getIndexCount());
}