#include "Geo_Sphere.hpp"

void Geo_SphereUV::genVertices(Eigen::Vector3f* data){
    Eigen::Vector3f topVertex = Eigen::Vector3f(0.0f, mShape3D.radius, 0.0f);
    *(data + 0) = topVertex; // first vertex is the top of the sphere

	unsigned v = 1;
	for(unsigned stack = 0; stack < mShape3D.xSegments - 1; stack++){
		double phi = TOPL_PI * double(stack + 1) / double(mShape3D.xSegments);
    	for (int slice = 0; slice < mShape3D.ySegments; slice++) {
			double theta = 2.0 * TOPL_PI * double(slice) / double(mShape3D.ySegments);
			double x = std::sin(phi) * std::cos(theta);
			double y = std::cos(phi);
			double z = std::sin(phi) * std::sin(theta);
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
	// TODO: Implement body here
	return;
}