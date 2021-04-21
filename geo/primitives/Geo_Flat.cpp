#include "Geo_Flat.hpp"

void Geo_Flat::genVertices(Eigen::Vector3f* data){
	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mShape2D.segments;

    Eigen::Vector3f centerVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL);
    *(data + 0) = centerVertex; // first vertex is the center vertex

    for(unsigned v = 1; v < mVertexCount; v++)
        *(data + v) = Eigen::Vector3f(
			sin(mStartAngle + (v * incAngle)) * mShape2D.radius, 
			cos(mStartAngle + (v * incAngle)) * mShape2D.radius, 
			DEFAULT_Z_VAL
        );
}

void Geo_Flat::genTexCoords(Eigen::Vector2f* data) {
	for (unsigned t = 0; t < mVertexCount; t++)
		*(data + t) = Eigen::Vector2f(0.0f, 0.0f); // Fix this!
}

void Geo_Flat::genIndices(unsigned* data){
	unsigned currentIdx; // increments as more indices are added
	// Indexing FRONT FACE
	unsigned currentVertex = 1; // starting from index 1, which is the rightmost point
	for (currentIdx = 0; currentIdx < mIndexCount - 3; currentIdx += 3) { // iterate to all but last trig!!!
		*(data + currentIdx + 0) = 0; // origin point
		*(data + currentIdx + 1) = currentVertex; // take the start vertex
		*(data + currentIdx + 2) = currentVertex + 1; // connect to next vertex

		currentVertex++;
	}

	// special case, connect to first vertex
	*(data + currentIdx + 0) = 0;
	*(data + currentIdx + 1) = currentVertex;
	*(data + currentIdx + 2) = 1; // connect back to first point
}