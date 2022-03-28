#include "Geo_Flat.hpp"

void Geo_Flat::genPos(Vec3f* data){
	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / _shape2D.segments;

    Vec3f centerVertex = Vec3f({ 0.0f, 0.0f, _depth });
    *(data + 0) = centerVertex; // first vertex is the center vertex

    for(unsigned v = 1; v < _verticesCount; v++)
        *(data + v) = Vec3f({
			(float)sin(_startAngle + ((v - 1) * incAngle)) * _shape2D.radius, 
			(float)cos(_startAngle + ((v - 1) * incAngle)) * _shape2D.radius, 
			(float)_depth
		});
}

void Geo_Flat::genNormals(Vec3f* data){
	Vec3f frontFaceNormal = Vec3f({ 0.0f, 0.0f, -1.0f });

	for(unsigned v = 1; v < _verticesCount; v++) *(data + v) = frontFaceNormal;
}

void Geo_Flat::genTexCoords(Vec2f* data) {
	// texture coordinates are based off of rectangular geometries

	*(data + 0) = Vec2f({ 0.5f, 0.5f }); // center point will always be shared
	for (unsigned t = 1; t < _verticesCount; t++)
		switch((t - 1) % 4){
			case 0: *(data + t) = Vec2f({ 1.0f, 0.0f }); break; // bottom left
			case 1: *(data + t) = Vec2f({ 0.0f, 0.0f }); break; // top left
			case 2: *(data + t) = Vec2f({ 0.0f, 1.0f }); break; // bottom right
			case 3: *(data + t) = Vec2f({ 1.0f, 1.0f }); break; // top right
		}
}

void Geo_Flat::genIndices(unsigned* data){
	unsigned i; // increments as more indices are added
	// Indexing FRONT FACE
	unsigned currentVertex = 1; // starting from index 1, which is the rightmost point
	for (i = 0; i < _indicesCount - 3; i += 3) { // iterate to all but last trig!!!
		*(data + i + 0) = 0; // origin point
		*(data + i + 1) = currentVertex; // take the start vertex
		*(data + i + 2) = currentVertex + 1; // connect to next vertex

		currentVertex++;
	}

	// special case, connect to first vertex
	*(data + i + 0) = 0;
	*(data + i + 1) = currentVertex;
	*(data + i + 2) = 1; // connect back to first point
}