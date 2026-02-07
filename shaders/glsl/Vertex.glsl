layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec3 texcoord_in;
layout(location = 2) in vec3 normal_in;
layout(location = 3) in vec3 vert_color_in;
layout(location = 4) in vec3 tangent_in;

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

vec3 getLook(vec2 angles){ return vec3(sin(-angles.y), sin(angles.x), -cos(angles.x + angles.y)); }

mat4 getLookAtMatrix(vec3 cPos, vec2 aRot, vec3 upPos){
	vec3 lPos = getLook(aRot);

	vec3 forward = normalize(lPos);
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

mat4 getCamMatrix(vec4 cPos, vec3 angles) { // placeholder camera
	// mat3 orientMatrix = getRotMatrix(angles);
	vec3 up = vec3(0.0, sin(angles.z), cos(angles.z));
	mat4 orientMatrix = getLookAtMatrix(vec3(cPos.x, cPos.y, cPos.z), vec2(angles.x, angles.y), up);

	/* mat4 camMatrix = mat4(
		orientMatrix[0][0], orientMatrix[0][1], orientMatrix[0][2], -cPos.x,
		orientMatrix[1][0], orientMatrix[1][1], orientMatrix[1][2], -cPos.y,
		orientMatrix[2][0], orientMatrix[2][1], orientMatrix[2][2], -cPos.z,
		0, 0, 0, 1
	); */

	mat4 camMatrix = mat4(
		1, 0, 0, -cPos.x,
		0, 1, 0, -cPos.y,
		0, 0, 1, -cPos.z,
		0, 0, 0, 1
	);

	if(angles.x != 0.0 || angles.y != 0.0 || angles.z != 0.0) return orientMatrix * camMatrix;
	else return camMatrix;
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

vec4 getVertex(vec3 target, vec3 translation, vec3 degrees, vec4 size){
	vec3 rotation = getRotMatrix(degrees) * target;
	vec4 pos = vec4(rotation, 1.0f) * size;
	pos += vec4(translation, 0.0f);

	return pos;
}