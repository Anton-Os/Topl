#include "Geo_Conic.hpp"

void Geo_Conic::genVertices(Eigen::Vector3f* data){
	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mShape2D.segments;
	const double startAngle = fullAngle / 4; // start at 90 degrees, pointing vertically

    Eigen::Vector3f centerVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL);
    *(data + 0) = centerVertex; // first vertex is the center vertex
	Eigen::Vector3f apexVertex = mApex;
	apexVertex[2] += DEFAULT_Z_VAL;
	// apexVertex += Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL);
	*(data + mVertexCount - 1) = apexVertex;

    for(unsigned v = 1; v < mVertexCount; v++)
        *(data + v) = Eigen::Vector3f(
			sin(startAngle + (v * incAngle)) * mShape2D.radius, 
			cos(startAngle + (v * incAngle)) * mShape2D.radius, 
			DEFAULT_Z_VAL
		);
}

void Geo_Conic::genNormals(Eigen::Vector3f* data){
	Eigen::Vector3f frontFaceNormal = Eigen::Vector3f(0.0f, 0.0f, -1.0f);
	Eigen::Vector3f backFaceNormal = Eigen::Vector3f(0.0f, 0.0f, 1.0f);

	*(data + mVertexCount - 1) = backFaceNormal; // back facing normal 
	for(unsigned v = 1; v < mVertexCount; v++) *(data + v) = frontFaceNormal;
}

void Geo_Conic::genTexCoords(Eigen::Vector2f* data) {
	// texture coordinates are based off of rectangular geometries

	*(data + 0) = Eigen::Vector2f(0.5f, 0.5f); // center point will always be shared
	for (unsigned t = 1; t < mVertexCount; t++)
		switch((t - 1) % 4){
			case 0: *(data + t) = Eigen::Vector2f(1.0f, 0.0f); break; // bottom left
			case 1: *(data + t) = Eigen::Vector2f(0.0f, 0.0f); break; // top left
			case 2: *(data + t) = Eigen::Vector2f(0.0f, 1.0f); break; // bottom right
			case 3: *(data + t) = Eigen::Vector2f(1.0f, 1.0f); break; // top right
		}
	*(data + mVertexCount - 1) = Eigen::Vector2f(0.5f, 0.5f); // apex point will always be shared
}

void Geo_Conic::genIndices(unsigned* data){
	unsigned currentIdx; // increments as more indices are added
	// Indexing FRONT FACE
	unsigned currentVertex = 1; // starting from index 1, which is the rightmost point

	for (currentIdx = 0; currentIdx < mIndexCount / 2 - 3; currentIdx += 3) { // iterate halfway through minus 1 trig
		*(data + currentIdx + 0) = 0; // center point
		*(data + currentIdx + 1) = currentVertex; // target vertex
		*(data + currentIdx + 2) = currentVertex + 1; // connect to next vertex
		currentVertex++;
	}

	// special case, connect to first vertex
	*(data + currentIdx + 0) = 0; // center point
	*(data + currentIdx + 1) = currentVertex;
	*(data + currentIdx + 2) = 1; // connect back to first point in sequence

	// Indexing APEX connection
	currentVertex = 1; // currentVertex needs to reset!
	for(currentIdx = mIndexCount / 2; currentIdx < mIndexCount - 3; currentIdx += 3){ // iterate to the end minus 1 trig
		*(data + currentIdx + 0) = mVertexCount - 1; // apex point
		*(data + currentIdx + 1) = currentVertex + 1; // connect to next vertex
		*(data + currentIdx + 1) = currentVertex; // target vertex
		currentVertex++;
	}

	// special case, connect to first vertex
	*(data + currentIdx + 0) = mIndexCount - 1; // apex point
	*(data + currentIdx + 1) = currentVertex;
	*(data + currentIdx + 2) = 1; // connect back to first point in sequence
}