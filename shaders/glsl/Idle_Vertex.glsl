#version 440

// Values

layout(std140, binding = 0) uniform Block{
	uint actorID;
};

layout(std140, binding = 0) uniform SceneBlock{
	uint mode;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

// Functions

mat3 calcRotMatrix(vec2 rotCoords) {
	mat3 zRotMatrix = mat3(1.0f); // begin with identity
	if(rotCoords.x != 0.0f) 
		zRotMatrix = mat3(
			cos(rotCoords.x), -1.0f * sin(rotCoords.x), 0,
			sin(rotCoords.x), cos(rotCoords.x), 0,
			0, 0, 1
		);

	mat yRotMatrix = mat3(1.0f); // begin with identity
	if(rotCoords.y != 0.0f)
		yRotMatrix = mat3(
			cos(rotCoords.y), 0, -1.0f * sin(rotCoords.y),
			0, 1, 0,
			sin(rotCoords.y), 0, cos(rotCoords.y)
		);

	return zRotMatrix * yRotMatrix;
}

mat4 calcCameraMatrix(vec3 cPos, vec3 lPos) {
	vec3 defaultUpVec = vec3(0.0, 1.0, 0.0);

	vec3 zAxis = normalize(cPos - lPos);
	vec3 xAxis = normalize(cross(defaultUpVec, zAxis));
	vec3 yAxis = cross(zAxis, xAxis);

	mat4 camMatrix = mat4(
		xAxis.x, xAxis.y, xAxis.z, -1.0 * dot(xAxis, cPos),
		yAxis.x, yAxis.y, yAxis.z, -1.0 * dot(yAxis, cPos),
		zAxis.x, zAxis.y, zAxis.z, -1.0 * dot(zAxis, cPos),
		0.0, 0.0, 0.0, 1.0
	);

	return camMatrix;
}

// Main

void main() {
	gl_Position = vec4(pos, 1.0f);
}