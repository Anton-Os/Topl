#include "Geo_Sphere.hpp"

void Geo_SphereUV::genVertices(Eigen::Vector3f* data){
    Eigen::Vector3f topVertex = Eigen::Vector3f(0.0f, mShape3D.radius, 0.0f);
    *(data + 0) = topVertex; // first vertex is the top of the sphere

	unsigned v = 1;
	for(unsigned stack = 0; stack < mShape3D.xSegments - 1; stack++){
		double phi = TOPL_PI * double(stack + 1) / double(mShape3D.xSegments);
    	for (unsigned slice = 0; slice < mShape3D.ySegments; slice++) {
			double theta = 2.0 * TOPL_PI * double(slice) / double(mShape3D.ySegments);
			double x = std::sin(phi) * std::cos(theta) * mShape3D.radius;
			double y = std::cos(phi) * mShape3D.radius;
			double z = std::sin(phi) * std::sin(theta) * mShape3D.radius;
			*(data + v) = Eigen::Vector3f(x, y, z);
			v++; // increase the vertex count
		}
	}

	Eigen::Vector3f botVertex = Eigen::Vector3f(0.0f, -1.0f * mShape3D.radius, 0.0f);
    *(data + mVertexCount - 1) = botVertex; // last vertex is the bottom of the sphere
}

void Geo_SphereUV::genNormals(Eigen::Vector3f* data){
	// TODO: Implement body here
	return;
}

void Geo_SphereUV::genTexCoords(Eigen::Vector2f* data) {
	// TODO: Implement body here
	return;
}

void Geo_SphereUV::genIndices(unsigned* data){
	// special cases, top and bottom vertices
	unsigned v = 1;
	unsigned i = 0;
	for (i = 0; i < mShape3D.ySegments * 6; i += 6) {
		// Top vertices
		*(data + i + 0) = 0; // top vertex
		*(data + i + 1) = v;
		*(data + i + 2) = (v % mShape3D.ySegments) + 1;

		// Bottom Vertices
		*(data + i + 3) = mVertexCount - 1;
		*(data + i + 4) = mVertexCount - (v + 1);
		*(data + i + 5) = (i < (mShape3D.ySegments * 6) - 6) ? mVertexCount - (v + 2) : mVertexCount - 2; // special case last iteration

		v++;
	}

	// sphere volumetric quads
	v = 1;
	for(i = i; i < ((mShape3D.xSegments - 1) * mShape3D.ySegments) * 6; i += 6){
		*(data + i + 0) = v;
		*(data + i + 1) = v + 1;
		*(data + i + 2) = v + mShape3D.xSegments + 1;

		*(data + i + 3) = v;
		*(data + i + 4) = v + mShape3D.xSegments;
		*(data + i + 5) = v + mShape3D.xSegments + 1;

		v++;
	}
}