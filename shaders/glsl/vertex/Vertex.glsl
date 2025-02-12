mat3 getRotMatrix(vec3 angles) {
	mat3 zRotMatrix = mat3( // Roll
		cos(angles.x), sin(angles.x), 0,
		-sin(angles.x), cos(angles.x), 0,
		0, 0, 1
	);

	mat3 xRotMatrix = mat3( // Pitch
		1, 0, 0,
		0, cos(angles.y), sin(angles.y),
		0, -sin(angles.y), cos(angles.y)
	);

	mat3 yRotMatrix = mat3( // Yaw
		cos(angles.z), 0, sin(angles.z),
		0, 1, 0,
		-1.0 * sin(angles.z), 0, cos(angles.z)
	);

	return zRotMatrix * yRotMatrix * xRotMatrix;
}

mat4 getCamMatrix(vec4 cPos, vec3 angles) { // placeholder camera
	return mat4(
		cos(angles.z) * cos(angles.x), -sin(angles.x), sin(angles.z), -cPos.x,
		sin(angles.x), cos(angles.x) * cos(angles.y), sin(angles.y), -cPos.y,
		-1.0 * sin(angles.z), -sin(angles.y), cos(angles.y) * cos(angles.z), -cPos.z,
		0, 0, 0, 1
	);
}

mat4 getLookAtMatrix(vec3 cPos, vec3 lPos, vec3 upPos){
	vec3 forward = normalize(lPos - cPos);
	vec3 right = normalize(cross(forward, upPos));
	vec3 up = normalize(cross(right, upPos));
	
	mat4 lookAtMatrix = mat4(
		right.x, right.y, right.z, -cPos.x,
		up.x, up.y, up.z, -cPos.y,
		-forward.x, -forward.y, -forward.z, -cPos.z,
		0, 0, 0, 1
	);

	return lookAtMatrix;
}

float getLineDistance(vec2 coord, vec2 p1, vec2 p2){
	return abs(((p2.y - p1.y) * coord.x) - ((p2.x - p1.x) * coord.y) + (p2.x * p1.y) - (p2.y * p1.x)) / sqrt(pow(p2.y - p1.y, 2.0) + pow(p2.x - p1.x, 2.0));
}

vec3 getCoordDistances(vec2 coord, vec2 p1, vec2 p2){
	return vec3(
		sqrt(pow(p2.x - p1.x, 2.0) + pow(p2.y - p1.y, 2.0)), // distance between points 1 and 2
		sqrt(pow(coord.x - p1.x, 2.0) + pow(coord.y - p1.y, 2.0)), // distance between coordinate and point 1 
		sqrt(pow(coord.x - p2.x, 2.0) + pow(coord.y - p2.y, 2.0)) // distance between coordinate and point 2
	);
}