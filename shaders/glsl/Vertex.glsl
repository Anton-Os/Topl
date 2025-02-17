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
	mat3 rotMatrix = getRotMatrix(angles);

	mat4 camMatrix = mat4(
		rotMatrix[0][0], rotMatrix[0][1], rotMatrix[0][2], -cPos.x,
		rotMatrix[1][0], rotMatrix[1][1], rotMatrix[1][2], -cPos.y,
		rotMatrix[2][0], rotMatrix[2][1], rotMatrix[2][2], -cPos.z,
		0, 0, 0, 1
	);

	return camMatrix;
}

mat4 getLookAtMatrix(vec3 cPos, vec3 lPos, vec3 upPos){
	// mat4 camMatrix = getCamMatrix(vec4(cPos, 1.0), lPos); // TODO: Compute based on camera value
	
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

vec4 getVertex(vec3 input, vec3 translation, vec3 degrees, vec4 size){
	vec3 rotation = getRotMatrix(degrees) * input;
	vec4 pos = vec4(rotation, 1.0f) * size;
	pos += vec4(translation, 0.0f);

	return pos;
}