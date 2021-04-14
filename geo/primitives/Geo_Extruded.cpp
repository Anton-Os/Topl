#include "Geo_Extruded.hpp"

void Geo_Extruded::genVertices(Eigen::Vector3f* data){
	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mShape2D.segments;

    Eigen::Vector3f frontCenterVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL);
    *(data + 0) = frontCenterVertex; // First vertex is the center vertex

    for(unsigned v = 1; v < mVertexCount / 2; v++) 
        *(data + v) = Eigen::Vector3f(
			sin(mStartAngle + (v * incAngle)) * mShape2D.radius, 
			cos(mStartAngle + (v * incAngle)) * mShape2D.radius, 
			DEFAULT_Z_VAL
		);

	Eigen::Vector3f backCenterVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL + mDepth);
	*(data + (mVertexCount / 2)) = backCenterVertex;

	for(unsigned v = (mVertexCount / 2) + 1; v < mVertexCount; v++)
		*(data + v) = Eigen::Vector3f(
			sin(mStartAngle + ((v - (mVertexCount / 2)) * incAngle)) * mShape2D.radius, 
			cos(mStartAngle + ((v - (mVertexCount / 2)) * incAngle)) * mShape2D.radius, 
			DEFAULT_Z_VAL
		);
}

void Geo_Extruded::genTexCoords(Eigen::Vector2f* data) {
	for (unsigned t = 0; t < mVertexCount; t++)
		*(data + t) = Eigen::Vector2f(0.0f, 0.0f); // Fix this!
}

void Geo_Extruded::genIndices(unsigned* data){
	return; // Fix this!
}