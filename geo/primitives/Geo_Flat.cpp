#include "Geo_Flat.hpp"

void Geo_Flat::genVertices(Eigen::Vector3f* data){
	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mShape2D.segments;

    Eigen::Vector3f centerVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL);
    *(data + 0) = centerVertex; // first vertex is the center vertex

    for(unsigned v = 1; v < mVertexCount; v++)
        *(data + v) = Eigen::Vector3f(
			sin(mStartAngle + ((v - 1) * incAngle)) * mShape2D.radius, 
			cos(mStartAngle + ((v - 1) * incAngle)) * mShape2D.radius, 
			DEFAULT_Z_VAL
        );
}

void Geo_Flat::genTexCoords(Eigen::Vector2f* data) {
	// texture coordinates are based off of rectangular geometries

	*(data + 0) = Eigen::Vector2f(0.5f, 0.5f); // center point will always be shared
	for (unsigned t = 1; t < mVertexCount; t++)
		switch((t - 1) % 4){
			case 0: *(data + t) = Eigen::Vector2f(1.0f, 0.0f); break; // bottom left
			case 1: *(data + t) = Eigen::Vector2f(0.0f, 0.0f); break; // top left
			case 2: *(data + t) = Eigen::Vector2f(0.0f, 1.0f); break; // bottom right
			case 3: *(data + t) = Eigen::Vector2f(1.0f, 1.0f); break; // top right
		}
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