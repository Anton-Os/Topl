#include "Geo_Orboid.hpp"

// Functions

Geo_Vertex* genOrb_vertices(Shape3D shape){
	unsigned count = (shape.xSegs + 1) * (shape.ySegs + 1);
	Geo_Vertex* vertexData = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

	unsigned v = 0;
	for (unsigned stack = 0; stack <= shape.xSegs; stack++) {
		float stackAngle = (MATH_PI / 2) - (stack * (MATH_PI / shape.xSegs));
		float xy = RADIUS_SIZE(shape.radius) * cosf(stackAngle);
		float z = RADIUS_SIZE(shape.radius) * sinf(stackAngle);
		for (unsigned slice = 0; slice <= shape.ySegs; slice++) {
			// float sliceAngle = slice * ((2 * MATH_PI) / shape.ySegments);
			float sliceAngle = slice * ANGLE_OFFSET(shape.ySegs);
			float x = xy * cosf(sliceAngle);
			float y = xy * sinf(sliceAngle);

			Vec3f pos = Vec3f({ x, y, z });
			*(vertexData + v) = Geo_Vertex(pos);
			v++; // increment to next vertex
		}
	}

	return vertexData;
}

unsigned* genOrb_indices(Shape3D shape){
	unsigned vCount = (shape.xSegs + 1) * (shape.ySegs + 1);
	unsigned iCount = ((shape.xSegs + 1) * (shape.ySegs + 1)) * 6;
	unsigned* indexData = (unsigned*)malloc(iCount * sizeof(unsigned));

	unsigned i = 0;
	for (unsigned stack = 0; stack <= shape.xSegs; stack++) {
		unsigned stackIndex = stack * (shape.ySegs + 1);
		unsigned nextIndex = stackIndex + shape.ySegs + 1;
		for (unsigned slice = 0; slice <= shape.ySegs; slice++) {
			stackIndex++;
			nextIndex++;

			*(indexData + i) = stackIndex;
			*(indexData + i + 1) = nextIndex;
			*(indexData + i + 2) = stackIndex + 1; // (stackIndex + 1) % shape.xSegs;
			i += 3;

			if (stack < shape.xSegs - 1) { // index all except last stack
				*(indexData + i) = stackIndex + 1; // (stackIndex + 1) % shape.xSegs;
				*(indexData + i + 1) = nextIndex;
				*(indexData + i + 2) = nextIndex + 1; // (nextIndex + 1) % shape.xSegs;
				i += 3;
			} else {
				// missing triangle
				*(indexData + i) = shape.xSegs;
				*(indexData + i + 1) = shape.xSegs + 1;
				*(indexData + i + 2) = shape.xSegs + 2;
			}
		}
	}

	return indexData;
}

Geo_Vertex* genTorus_vertices(Shape3D shape, float d){
	unsigned count = (shape.xSegs + 1) * (shape.ySegs + 1);
	Geo_Vertex* vertexData = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

	float x, y, z, xy;
	float sectorStep = (2 * MATH_PI) / shape.xSegs;
	float sideStep = (2 * MATH_PI) / shape.ySegs;

	unsigned v = 0;
	for(unsigned innerRing = 0; innerRing <= shape.xSegs; innerRing++){
		float sideAngle = MATH_PI - (innerRing * sideStep);
		xy = (shape.radius * d) * cosf(sideAngle);
		z = (shape.radius * d) * sinf(sideAngle);

		for(unsigned outerRing = 0; outerRing < shape.ySegs; outerRing++){
			float sectorAngle = outerRing * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);

			x += shape.radius * cosf(sectorAngle);
			y += shape.radius * sinf(sectorAngle);

			*(vertexData + v) = Geo_Vertex({ x, y, z });
			v++;
		}
	}

	return vertexData;
}

unsigned* genTorus_indices(Shape3D shape){
	unsigned vCount = (shape.xSegs + 1) * (shape.ySegs + 1);
	unsigned iCount = ((shape.xSegs + 1) * (shape.ySegs + 1)) * 6;
	unsigned* indexData = (unsigned*)malloc(iCount * sizeof(unsigned));

	unsigned i = 0;

	for(unsigned innerRing = 0; innerRing <= shape.xSegs; innerRing++){
		unsigned idx = innerRing * (shape.ySegs + 1);
		unsigned nextIdx = idx + shape.ySegs + 1;

		for(unsigned outerRing = 0; outerRing < shape.ySegs + 1; outerRing++){
			*(indexData + i + 0) = idx;
			*(indexData + i + 1) = nextIdx;
			*(indexData + i + 2) = idx + 1;
			*(indexData + i + 3) = idx + 1;
			*(indexData + i + 4) = nextIdx;
			*(indexData + i + 5) = nextIdx + 1;
			i += 6;
			idx++; nextIdx++;
		}
	}

	return indexData;
}