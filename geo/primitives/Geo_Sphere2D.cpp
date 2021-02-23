#include "Geo_Sphere2D.hpp"

Eigen::Vector3f* Geo_Sphere2D::genVertices(){
	if (getVData() != nullptr) cleanup();  // This means an attempt was made to override code
	// TODO: This code should be in the base class!!!!

    Eigen::Vector3f* data = (Eigen::Vector3f*)malloc(mVCount * sizeof(Eigen::Vector3f));

	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mCircle.segments;
	const double startAngle = fullAngle / 4; // Start at 90 degrees, pointing vertically

    *(data + 0) = Eigen::Vector3f(0.0f, 0.0f, 0.0f); // Circle origin
    for(unsigned v = 1; v < mVCount; v++) // We have already created
        *(data + v) = Eigen::Vector3f(
								sin(startAngle + (v * incAngle)) * mCircle.radius, 
								cos(startAngle + (v * incAngle)) * mCircle.radius, 
								DEFAULT_Z_VAL);

    return data;
}

Eigen::Vector2f* Geo_Sphere2D::genTexCoords() {
	if (getTData() != nullptr) cleanup();  // This means an attempt was made to override code
	// TODO: This code should be in the base class!!!!

	Eigen::Vector2f* data = (Eigen::Vector2f*)malloc(mVCount * sizeof(Eigen::Vector2f));

	for (unsigned t = 0; t < mVCount; t++)
		*(data + t) = Eigen::Vector2f(0.0f, 0.0f); 

	return data;
}

unsigned* Geo_Sphere2D::genIndices(){
	if (getIData() != nullptr) cleanup();  // This means an attempt was made to override code
	// TODO: This code should be in the base class!!!!
    unsigned* data = (unsigned*)malloc(mICount * sizeof(unsigned));

	unsigned startCVert = 1; // Starting from index 1, which is the rightmost point
	unsigned currentIdx; // Will use this in the for loop
	for (currentIdx = 0; currentIdx < mICount - 3; currentIdx += 3) { // Iterate to all but last trig!!!
		*(data + currentIdx + 0) = 0; // Origin point
		*(data + currentIdx + 1) = startCVert; // Take the start vertex
		*(data + currentIdx + 2) = startCVert + 1; // Connect to next vertex

		startCVert++;
	}

	// Adressing last point, SPECIAL CASE
	*(data + currentIdx + 0) = 0;
	*(data + currentIdx + 1) = startCVert;
	*(data + currentIdx + 2) = 1; // Connect back to first point

    return data;
}