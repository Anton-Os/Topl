#include "Geo_Sphere.hpp"

// Sphere Implementation

void Geo_Sphere::genPos(Vec3f* data){
	// checkout http://www.songho.ca/opengl/gl_sphere.html
	const float radius = _shape3D.radius * RADIAL_UNITS;
	unsigned v = 0; // target vertex

	for(unsigned stack = 0; stack <= _shape3D.xSegments; stack++){
		float stackAngle = (TOPL_PI / 2) - (stack * (TOPL_PI / _shape3D.xSegments));
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);
    	for (unsigned slice = 0; slice <= _shape3D.ySegments; slice++) {
			float sliceAngle = slice * ((2 * TOPL_PI) / _shape3D.ySegments);
			float x = xy * cosf(sliceAngle);
			float y = xy * sinf(sliceAngle);

			*(data + v) = Vec3f({ x, y, z }); // assignment
			v++; // increment to next vertex
		}
	}
}

void Geo_Sphere::genNormals(Vec3f* data){
	unsigned n = 0; // target vertex

	for(unsigned stack = 0; stack <= _shape3D.xSegments; stack++){
		float stackAngle = (TOPL_PI / 2) - (stack * (TOPL_PI / _shape3D.xSegments));
		float xy = cosf(stackAngle);
		float z = sinf(stackAngle);
    	for (unsigned slice = 0; slice <= _shape3D.ySegments; slice++) {
			float sliceAngle = slice * ((2 * TOPL_PI) / _shape3D.ySegments);
			float x = xy * cosf(sliceAngle);
			float y = xy * sinf(sliceAngle);

			Vec3f normal = Vec3f({ x, y, z });
			normal.normalize();
			*(data + n) = normal; // assignment
			n++; // increment to next vertex
		}
	}
}

void Geo_Sphere::genTexCoords(Vec2f* data) { 
	unsigned t = 0;

	for(unsigned stack = 0; stack <= _shape3D.xSegments; stack++){
    	for (unsigned slice = 0; slice <= _shape3D.ySegments; slice++) {
			Vec2f texcoord = Vec2f({
				(float)(slice / _shape3D.ySegments), 
				(float)(stack / _shape3D.xSegments)
			});

			*(data + t) = texcoord; // assignment
			t++; // increment to next vertex
		}
	}
}

void Geo_Sphere::genIndices(unsigned* data){
	unsigned i = 0;

	for(unsigned stack = 0; stack <= _shape3D.xSegments; stack++){
		unsigned stackIndex = stack * (_shape3D.ySegments + 1);
		unsigned nextIndex = stackIndex + _shape3D.ySegments + 1;
    	for (unsigned slice = 0; slice <= _shape3D.ySegments; slice++) {
			stackIndex++;
			nextIndex++;

			if(stack != 0){ // index all except first stack
				*(data + i) = stackIndex;
				*(data + i + 1) = nextIndex;
				*(data + i + 2) = stackIndex + 1;
				i += 3;
			}
			if(stack < _shape3D.xSegments - 1){ // index all except last stack
				*(data + i) = stackIndex + 1;
				*(data + i + 1) = nextIndex;
				*(data + i + 2) = nextIndex + 1;
				i += 3;
			}
		}
	}

	i = i;
	return;
}

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