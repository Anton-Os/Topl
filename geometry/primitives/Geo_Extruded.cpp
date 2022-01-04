#include "Geo_Extruded.hpp"

void Geo_Extruded::genVertices(Eigen::Vector3f* data){
	// Vertices for FRONT FACE
	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / _shape2D.segments;

    Eigen::Vector3f centerVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL + (_depth / 2));
    *(data + 0) = centerVertex; // first vertex is the center vertex

    for(unsigned v = 1; v < _verticesCount / 2; v++)
        *(data + v) = Eigen::Vector3f(
			sin(_startAngle + (v * incAngle)) * _shape2D.radius, 
			cos(_startAngle + (v * incAngle)) * _shape2D.radius, 
			DEFAULT_Z_VAL + (_depth / 2)
        );

	// Vertices for BACK FACE
    centerVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL - (_depth / 2));
    *(data + (_verticesCount / 2)) = centerVertex; // first vertex is the center vertex

    for(unsigned v = 1 + (_verticesCount / 2); v < _verticesCount; v++)
        *(data + v) = Eigen::Vector3f(
			sin(_startAngle + ((v - (_verticesCount / 2)) * incAngle)) * _shape2D.radius,
			cos(_startAngle + ((v - (_verticesCount / 2)) * incAngle)) * _shape2D.radius,
			DEFAULT_Z_VAL - (_depth / 2)
        );
}

void Geo_Extruded::genNormals(Eigen::Vector3f* data){
	Eigen::Vector3f frontFaceNormal = Eigen::Vector3f(0.0f, 0.0f, -1.0f);
	Eigen::Vector3f backFaceNormal = Eigen::Vector3f(0.0f, 0.0f, 1.0f);

	for(unsigned v = 1; v < _verticesCount / 2; v++) *(data + v) = frontFaceNormal;
	for(unsigned v = 1 + (_verticesCount / 2); v < _verticesCount; v++) *(data + v) = backFaceNormal;
}

void Geo_Extruded::genTexCoords(Eigen::Vector2f* data) {
	// texture coordinates are based off of rectangular geometries

	// Texcoords for FRONT FACE
	*(data + 0) = Eigen::Vector2f(0.5f, 0.5f); // center point will always be shared
	for (unsigned t = 1; t < _verticesCount; t++)
		switch((t - 1) % 4){
			case 0: *(data + t) = Eigen::Vector2f(1.0f, 0.0f); break; // bottom left
			case 1: *(data + t) = Eigen::Vector2f(0.0f, 0.0f); break; // top left
			case 2: *(data + t) = Eigen::Vector2f(0.0f, 1.0f); break; // bottom right
			case 3: *(data + t) = Eigen::Vector2f(1.0f, 1.0f); break; // top right
		}

	// Texcoords for BACK FACE
	*(data + (_verticesCount / 2)) = Eigen::Vector2f(0.5f, 0.5f); // center point will always be shared
	for(unsigned t = 1 + (_verticesCount / 2); t < _verticesCount; t++)
        switch((t - 1) % 4){
			case 0: *(data + t) = Eigen::Vector2f(1.0f, 0.0f); break; // bottom left
			case 1: *(data + t) = Eigen::Vector2f(0.0f, 0.0f); break; // top left
			case 2: *(data + t) = Eigen::Vector2f(0.0f, 1.0f); break; // bottom right
			case 3: *(data + t) = Eigen::Vector2f(1.0f, 1.0f); break; // top right
		}
}

void Geo_Extruded::genIndices(unsigned* data){
	unsigned i; // increments as more indices are added
	// Indexing FRONT FACE
	unsigned v = 1; // starting from index 1, which is the rightmost point
	// for (i = 0; i < (_indicesCount / 2) - 3; i += 3) { // iterate to all but last trig!!!
	for (i = 0; i < (_indicesCount / 4) - 3; i += 3) { // iterate to all but last trig!!!
		*(data + i + 0) = 0; // origin point
		*(data + i + 1) = v; // take the start vertex
		*(data + i + 2) = v + 1; // connect to next vertex

		v++;
	}

	// special case, last trig
	*(data + i + 0) = 0;
	*(data + i + 1) = v;
	*(data + i + 2) = 1; // connect back to first point of FRONT FACE

	// Indexing BACK FACE
	v = _verticesCount / 2 + 1; // target the first edge vertex of the back face
	// for (i = _indicesCount / 2; i < _indicesCount - 3; i += 3) { // iterate to all but last trig!!!
	for (i = _indicesCount / 4; i < (_indicesCount / 2) - 3; i += 3) {
		*(data + i + 0) = _verticesCount / 2; // origin point
		*(data + i + 1) = v; // take the start vertex
		*(data + i + 2) = v + 1; // connect to next vertex

		v++;
	}

	// special case, last trig
	*(data + i + 0) = _verticesCount / 2; // center point of BACK FACE
	*(data + i + 1) = v;
	*(data + i + 2) = _verticesCount / 2 + 1; // connect back to first point of BACK FACE

	// Indexing SIDE FACES
	
	v = 1; // starting from index 1 which is a corner point
	for (i = _indicesCount / 2; i < _indicesCount - 6; i += 6) {
		*(data + i + 0) = v;
		*(data + i + 2) = v + (_verticesCount / 2) + 1;
		*(data + i + 1) = v + 1;

		*(data + i + 3) = v + (_verticesCount / 2);
		*(data + i + 4) = v;
		*(data + i + 5) = v + (_verticesCount / 2) + 1;

		v++; // increment current vertex
	}

	*(data + i + 0) = 1;
	*(data + i + 1) = (_verticesCount / 2) + 1;
	*(data + i + 2) = (_verticesCount / 2) - 1;

	*(data + i + 3) = _verticesCount - 1;
	*(data + i + 4) = (_verticesCount / 2) - 1;
	*(data + i + 5) = (_verticesCount / 2) + 1;
}