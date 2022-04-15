#include "Geo_Sphere.hpp"

// Sphere Implementation

void Geo_Sphere::genPos(Vec3f* data){
	const float radius = _shape3D.radius * RADIAL_UNITS;

    Vec3f topVertex = Vec3f({ 0.0f, radius, 0.0f });
    *(data + 0) = topVertex; // first vertex is the top of the sphere

	unsigned v = 1;
	for(unsigned stack = 0; stack < _shape3D.xSegments - 1; stack++){
		double phi = TOPL_PI * double(stack + 1) / double(_shape3D.xSegments);
    	for (unsigned slice = 0; slice < _shape3D.ySegments; slice++) {
			double theta = 2.0 * TOPL_PI * double(slice) / double(_shape3D.ySegments);
			double x = sin(phi) * cos(theta) * radius;
			double y = cos(phi) * radius;
			double z = sin(phi) * sin(theta) * radius;
			*(data + v) = Vec3f({ (float)x, (float)y, (float)z });
			v++; // increase the vertex count
		}
	}

	Vec3f botVertex = Vec3f({ 0.0f, -1.0f * radius, 0.0f });
    *(data + _vertexCount - 1) = botVertex; // last vertex is the bottom of the sphere
}

void Geo_Sphere::genNormals(Vec3f* data){ return; }

void Geo_Sphere::genTexCoords(Vec2f* data) { return; }

void Geo_Sphere::genIndices(unsigned* data){ return; }

// IcoSphere Implmentation

void Geo_IcoSphere::genPos(Vec3f* data){
	return;
}

void Geo_IcoSphere::genNormals(Vec3f* data){
	return;
}

void Geo_IcoSphere::genTexCoords(Vec2f* data) {
	return;
}

void Geo_IcoSphere::genIndices(unsigned* data){
	return;
}