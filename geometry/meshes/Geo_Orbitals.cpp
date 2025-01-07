#include "Geo_Orbitals.hpp"

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

			if (stack != 0 ) { // index all except first stack
				_indices[i] = stackIndex;
				_indices[i + 1] = nextIndex;
				_indices[i + 2] = stackIndex + 1; // (stackIndex + 1) % _shape.xSegs;
				i += 3;
			}
			if (stack < _shape.xSegs - 1) { // index all except last stack
				_indices[i] = stackIndex + 1; // (stackIndex + 1) % _shape.xSegs;
				_indices[i + 1] = nextIndex;
				_indices[i + 2] = nextIndex + 1; // (nextIndex + 1) % _shape.xSegs;
				i += 3;
			}
		}

		// Perform indexing for missing piece?
	}
}

void Geo_Torus::init(){
	unsigned v = 0;
	float radiusInc, z;

	for(unsigned innerRing = 0; innerRing < _shape.xSegs; innerRing++){
		radiusInc = _shape.radius + ((_shape.radius * _diameter) * cosf((MATH_PI / _shape.xSegs) * innerRing));
		z = (_shape.radius * _diameter) * sinf((MATH_PI / _shape.xSegs) * innerRing);

		for(unsigned outerRing = 0; outerRing < _shape.ySegs; outerRing++){
			float x = radiusInc * cosf((MATH_PI / _shape.ySegs) * outerRing);
			float y = radiusInc * sinf((MATH_PI / _shape.ySegs) * outerRing);
			
			Vec3f pos = Vec3f({ x, y, z });
			_vertices[v] = Geo_Vertex(pos);
			v++;
		}
	}

	// TODO: Perform Indexing
}