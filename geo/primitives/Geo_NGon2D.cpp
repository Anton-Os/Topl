#include "Geo_NGon2D.hpp"

void Geo_NGon2D::genVertices(Eigen::Vector3f* data){
	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mNGon.segments;
	const double startAngle = fullAngle / 4; // Start at 90 degrees, pointing vertically

    *(data + 0) = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL); // Origin
    for(unsigned v = 1; v < mVertexCount; v++) // We have already created
        *(data + v) = Eigen::Vector3f(
								sin(startAngle + (v * incAngle)) * mNGon.radius, 
								cos(startAngle + (v * incAngle)) * mNGon.radius, 
								DEFAULT_Z_VAL);
}

void Geo_NGon2D::genTexCoords(Eigen::Vector2f* data) {
	for (unsigned t = 0; t < mVertexCount; t++)
		*(data + t) = Eigen::Vector2f(0.0f, 0.0f);
}

void Geo_NGon2D::genIndices(unsigned* data){
	unsigned startCVert = 1; // Starting from index 1, which is the rightmost point
	unsigned currentIdx; // Will use this in the for loop
	for (currentIdx = 0; currentIdx < mIndexCount - 3; currentIdx += 3) { // Iterate to all but last trig!!!
		*(data + currentIdx + 0) = 0; // Origin point
		*(data + currentIdx + 1) = startCVert; // Take the start vertex
		*(data + currentIdx + 2) = startCVert + 1; // Connect to next vertex

		startCVert++;
	}

	// Adressing last point, SPECIAL CASE
	*(data + currentIdx + 0) = 0;
	*(data + currentIdx + 1) = startCVert;
	*(data + currentIdx + 2) = 1; // connect back to first point
}