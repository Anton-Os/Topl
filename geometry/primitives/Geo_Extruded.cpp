#include "Geo_Extruded.hpp"

void Geo_Extruded::genPos(Vec3f* data){
	const double angle = (MATH_PI * 2) / _shape2D.segments;
	const float radius = _shape2D.radius * RADIAL_UNITS;
	
	// Vertices for FRONT FACE

    Vec3f centerVertex = Vec3f({ 0.0f, 0.0f, DEFAULT_Z + (_depth / 2)});
    *(data + 0) = centerVertex; // first vertex is the center vertex

    for(unsigned v = 1; v < _vertexCount / 2; v++)
        *(data + v) = Vec3f({
			(float)sin(_startAngle + (v * angle)) * radius, 
			(float)cos(_startAngle + (v * angle)) * radius, 
			(float)DEFAULT_Z + (_depth / 2)
		});

	// Vertices for BACK FACE
    centerVertex = Vec3f({0.0f, 0.0f, DEFAULT_Z - (_depth / 2)});
    *(data + (_vertexCount / 2)) = centerVertex; // first vertex is the center vertex

    for(unsigned v = 1 + (_vertexCount / 2); v < _vertexCount; v++)
        *(data + v) = Vec3f({
			(float)sin(_startAngle + ((v - (_vertexCount / 2)) * angle)) * radius,
			(float)cos(_startAngle + ((v - (_vertexCount / 2)) * angle)) * radius,
			(float)DEFAULT_Z - (_depth / 2)
		});
}

void Geo_Extruded::genNormals(Vec3f* data){
	const Vec3f frontNormalVec = Vec3f({ 0.0f, 0.0f, -1.0f });
	const Vec3f backNormalVec = Vec3f({ 0.0f, 0.0f, 1.0f });

	for(unsigned v = 1; v < _vertexCount / 2; v++) *(data + v) = frontNormalVec;
	for(unsigned v = 1 + (_vertexCount / 2); v < _vertexCount; v++) *(data + v) = backNormalVec;
}

void Geo_Extruded::genTexCoords(Vec2f* data) {
	// Texcoords for FRONT FACE
	*(data + 0) = Vec2f({ 0.5f, 0.5f }); // center point will always be shared
	for (unsigned t = 1; t < _vertexCount; t++)
		switch ((t - 1) % 4) {
			case 0: *(data + t) = Vec2f({ 1.0f, 0.0f }); break; // bottom left
			case 1: *(data + t) = Vec2f({ 0.0f, 0.0f }); break; // top left
			case 2: *(data + t) = Vec2f({ 0.0f, 1.0f }); break; // bottom right
			case 3: *(data + t) = Vec2f({ 1.0f, 1.0f }); break; // top right
		}

	// Texcoords for BACK FACE
	*(data + (_vertexCount / 2)) = Vec2f({ 0.5f, 0.5f }); // center point will always be shared
	for(unsigned t = 1 + (_vertexCount / 2); t < _vertexCount; t++)
		switch ((t - 1) % 4) {
			case 0: *(data + t) = Vec2f({ 1.0f, 0.0f }); break; // bottom left
			case 1: *(data + t) = Vec2f({ 0.0f, 0.0f }); break; // top left
			case 2: *(data + t) = Vec2f({ 0.0f, 1.0f }); break; // bottom right
			case 3: *(data + t) = Vec2f({ 1.0f, 1.0f }); break; // top right
		}
}

void Geo_Extruded::genIndices(unsigned* data){
	unsigned i; // increments as more indices are added

	// Indexing FRONT FACE
	unsigned v = 1;
	for (i = 0; i < (_indexCount / 4) - 3; i += 3) {
		*(data + i + 0) = 0; // origin point
		*(data + i + 1) = v; // take the start vertex
		*(data + i + 2) = v + 1; // connect to next vertex

		v++;
	}

	// special case
	*(data + i + 0) = 0;
	*(data + i + 1) = v;
	*(data + i + 2) = 1;

	// Indexing BACK FACE
	v = _vertexCount / 2 + 1;
	for (i = _indexCount / 4; i < (_indexCount / 2) - 3; i += 3) {
		*(data + i + 0) = _vertexCount / 2; // origin point
		*(data + i + 1) = v; // take the start vertex
		*(data + i + 2) = v + 1; // connect to next vertex

		v++;
	}

	// special case
	*(data + i + 0) = _vertexCount / 2; // center point of BACK FACE
	*(data + i + 1) = v;
	*(data + i + 2) = _vertexCount / 2 + 1; // connect back to first point of BACK FACE

	// Indexing SIDE FACES
	v = 1;
	for (i = _indexCount / 2; i < _indexCount - 6; i += 6) {
		*(data + i + 0) = v;
		*(data + i + 2) = v + (_vertexCount / 2) + 1;
		*(data + i + 1) = v + 1;

		*(data + i + 3) = v + (_vertexCount / 2);
		*(data + i + 4) = v;
		*(data + i + 5) = v + (_vertexCount / 2) + 1;

		v++; // increment current vertex
	}

	// special case
	*(data + i + 0) = 1;
	*(data + i + 1) = (_vertexCount / 2) + 1;
	*(data + i + 2) = (_vertexCount / 2) - 1;

	// special case
	*(data + i + 3) = _vertexCount - 1;
	*(data + i + 4) = (_vertexCount / 2) - 1;
	*(data + i + 5) = (_vertexCount / 2) + 1;
}