#include "Geo_Flat.hpp"

void Geo_Flat::genVertices(Eigen::Vector3f* data){
	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mShape2D.segments;
	const double startAngle = fullAngle / 4; // Start at 90 degrees, pointing vertically

    Eigen::Vector3f centerVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL);
    *(data + 0) = centerVertex; // First vertex is the center vertex

    for(unsigned v = 1; v < mVertexCount; v++) // We have already created
        *(data + v) = Eigen::Vector3f(
			sin(startAngle + (v * incAngle)) * mShape2D.radius, 
			cos(startAngle + (v * incAngle)) * mShape2D.radius, 
			DEFAULT_Z_VAL
        );
}

void Geo_Flat::genTexCoords(Eigen::Vector2f* data) {
	for (unsigned t = 0; t < mVertexCount; t++)
		*(data + t) = Eigen::Vector2f(0.0f, 0.0f); // Fix this!
}

void Geo_Flat::genIndices(unsigned* data){
	unsigned currentVertex = 1; // Starting from index 1, which is the rightmost point
	unsigned currentIdx; // Will use this in the for loop
	for (currentIdx = 0; currentIdx < mIndexCount - 3; currentIdx += 3) { // Iterate to all but last trig!!!
		*(data + currentIdx + 0) = 0; // Origin point
		*(data + currentIdx + 1) = currentVertex; // Take the start vertex
		*(data + currentIdx + 2) = currentVertex + 1; // Connect to next vertex

		currentVertex++;
	}

	// Special Case, connect to first vertex
	*(data + currentIdx + 0) = 0;
	*(data + currentIdx + 1) = currentVertex;
	*(data + currentIdx + 2) = 1; // Connect back to first point
}