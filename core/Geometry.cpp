#include "Geometry.hpp"

#include <cmath>
// #include <math.h>
#include <cstdlib>
#include <cstdio>

// Geo_Rect2D Implementation

Eigen::Vector3f* Geo_Rect2D::genVertices(){
    Eigen::Vector3f* data = (Eigen::Vector3f*)malloc(mVCount * sizeof(Eigen::Vector3f));
    
    // The depth position of the vector is arbitrary for now
    Eigen::Vector3f topRight(mRect.height / 2.0f, mRect.width / 2.0f, 0.5f);
    Eigen::Vector3f topLeft(mRect.height / 2.0f, -1 * (mRect.width / 2.0f), 0.5f);
    Eigen::Vector3f botRight(-1 * (mRect.height / 2.0f), mRect.width / 2.0f, 0.5f);
    Eigen::Vector3f botLeft(-1 * (mRect.height / 2.0f), -1 * (mRect.width / 2.0f), 0.5f); 

    *(data + 0) = botLeft;
    *(data + 1) = botRight;
    *(data + 2) = topLeft;
    *(data + 3) = topRight;

    return data; // FIX! FIX! FIX!
}

unsigned* Geo_Rect2D::genIndices(){
    unsigned* data = (unsigned*)malloc(mICount * sizeof(unsigned));

    unsigned indicesArray[6] = { 
        0, 1, 2, 
        3, 2, 1
    };

    // Simply copies from array to to the allocated data
    for(unsigned i = 0; i < 6; i++)
        *(data + i) = indicesArray[i];

    return data;
}

// Geo_Sphere2D Implementation

Eigen::Vector3f* Geo_Sphere2D::genVertices(){
    Eigen::Vector3f* data = (Eigen::Vector3f*)malloc(mVCount * sizeof(Eigen::Vector3f));

	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mCircle.segments;
	const double startAngle = fullAngle / 4; // Start at 90 degrees, pointing vertically

    *(data + 0) = Eigen::Vector3f(0.0f, 0.0f, 0.0f); // Circle origin
    for(unsigned v = 1; v < mVCount; v++) // We have already created
        *(data + v) = Eigen::Vector3f(
								sin(startAngle + (v * incAngle)) * mCircle.radius, 
								cos(startAngle + (v * incAngle)) * mCircle.radius, 
								0.5f);

    return data;
}

unsigned* Geo_Sphere2D::genIndices(){
    unsigned* data = (unsigned*)malloc(mICount * sizeof(unsigned));

	/* for (unsigned i = 0; i < mICount; i += 3) { // We are setting 3 indices in one go
		*(data + i + 0) = 0; // Origin point
		*(data + i + 1) = startCVert; // Take the start vertex
		*(data + i + 2) = startCVert + 1; // Connect to next vertex

		startCVert++;
	} */

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