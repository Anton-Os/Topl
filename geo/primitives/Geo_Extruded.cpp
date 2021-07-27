#include "Geo_Extruded.hpp"

void Geo_Extruded::genVertices(Eigen::Vector3f* data){
	// Vertices for FRONT FACE
	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mShape2D.segments;

    Eigen::Vector3f centerVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL + (mDepth / 2));
    *(data + 0) = centerVertex; // first vertex is the center vertex

    for(unsigned v = 1; v < mVertexCount / 2; v++)
        *(data + v) = Eigen::Vector3f(
			sin(mStartAngle + (v * incAngle)) * mShape2D.radius, 
			cos(mStartAngle + (v * incAngle)) * mShape2D.radius, 
			DEFAULT_Z_VAL + (mDepth / 2)
        );

	// Vertices for BACK FACE
    centerVertex = Eigen::Vector3f(0.0f, 0.0f, DEFAULT_Z_VAL - (mDepth / 2));
    *(data + (mVertexCount / 2)) = centerVertex; // first vertex is the center vertex

    for(unsigned v = 1 + (mVertexCount / 2); v < mVertexCount; v++)
        *(data + v) = Eigen::Vector3f(
			sin(mStartAngle + (v * incAngle)) * mShape2D.radius, 
			cos(mStartAngle + (v * incAngle)) * mShape2D.radius, 
			DEFAULT_Z_VAL - (mDepth / 2)
        );
}

void Geo_Extruded::genNormals(Eigen::Vector3f* data){
	Eigen::Vector3f frontFaceNormal = Eigen::Vector3f(0.0f, 0.0f, -1.0f);
	Eigen::Vector3f backFaceNormal = Eigen::Vector3f(0.0f, 0.0f, 1.0f);

	for(unsigned v = 1; v < mVertexCount / 2; v++) *(data + v) = frontFaceNormal;
	for(unsigned v = 1 + (mVertexCount / 2); v < mVertexCount; v++) *(data + v) = backFaceNormal;
}

void Geo_Extruded::genTexCoords(Eigen::Vector2f* data) {
	// texture coordinates are based off of rectangular geometries

	// Texcoords for FRONT FACE
	*(data + 0) = Eigen::Vector2f(0.5f, 0.5f); // center point will always be shared
	for (unsigned t = 1; t < mVertexCount; t++)
		switch((t - 1) % 4){
			case 0: *(data + t) = Eigen::Vector2f(1.0f, 0.0f); break; // bottom left
			case 1: *(data + t) = Eigen::Vector2f(0.0f, 0.0f); break; // top left
			case 2: *(data + t) = Eigen::Vector2f(0.0f, 1.0f); break; // bottom right
			case 3: *(data + t) = Eigen::Vector2f(1.0f, 1.0f); break; // top right
		}

	// Texcoords for BACK FACE
	*(data + (mVertexCount / 2)) = Eigen::Vector2f(0.5f, 0.5f); // center point will always be shared
	for(unsigned t = 1 + (mVertexCount / 2); t < mVertexCount; t++)
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
	// for (i = 0; i < (mIndexCount / 2) - 3; i += 3) { // iterate to all but last trig!!!
	for (i = 0; i < (mIndexCount / 4) - 3; i += 3) { // iterate to all but last trig!!!
		*(data + i + 0) = 0; // origin point
		*(data + i + 1) = v; // take the start vertex
		*(data + i + 2) = v + 1; // connect to next vertex

		v++;
	}

	// special case, connect to first vertex
	*(data + i + 0) = 0;
	*(data + i + 1) = v;
	*(data + i + 2) = 1; // connect back to first point of FRONT FACE

	// Indexing BACK FACE
	v = mVertexCount / 2 + 1; // target the first edge vertex of the back face
	// for (i = mIndexCount / 2; i < mIndexCount - 3; i += 3) { // iterate to all but last trig!!!
	for (i = mIndexCount / 4; i < (mIndexCount / 2) - 3; i += 3) {
		*(data + i + 0) = mVertexCount / 2; // origin point
		*(data + i + 1) = v; // take the start vertex
		*(data + i + 2) = v + 1; // connect to next vertex

		v++;
	}

	// special case, connect to first vertex
	*(data + i + 0) = mVertexCount / 2; // center point of BACK FACE
	*(data + i + 1) = v;
	*(data + i + 2) = mVertexCount / 2 + 1; // connect back to first point of BACK FACE

	// Indexing SIDE FACES
	
	/* v = 1; // starting from index 1 which is a corner point
	for (i = mIndexCount / 2; i < mIndexCount - 6; i += 6) {
		*(data + i + 0) = v;
		*(data + i + 1) = v + (mVertexCount / 2);
		*(data + i + 2) = v + 1;

		*(data + i + 4) = v + (mVertexCount / 2) + 1;
		*(data + i + 5) = v + (mVertexCount / 2);
		*(data + i + 6) = v + 1;

		// v++; // increment current vertex
	}

	*(data + i + 0) = 1;
	*(data + i + 1) = (mVertexCount / 2) + 1;
	*(data + i + 2) = (mVertexCount / 2) + 2;

	*(data + i + 3) = (mVertexCount / 2) + 1;
	*(data + i + 4) = (mVertexCount / 2) + 2;
	*(data + i + 5) = mVertexCount - 1; */
}