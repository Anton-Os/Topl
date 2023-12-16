#version 440

#define LOOK_RADIUS 2

// Values

layout(std140, binding = 0) uniform Block {
	vec4 color;
	vec3 offset;
	vec3 rotation;
};

layout(std140, binding = 1) uniform SceneBlock {
	uint mode;
	vec3 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out uint vertexNo;
layout(location = 1) out vec3 pos_out;

// Functions

mat3 calcRotMatrix(vec3 angles){
	mat3 zRotMatrix = mat3( // Roll
		cos(angles.x), -sin(angles.x), 0,
		sin(angles.x), cos(angles.x), 0,
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

mat4 calcCamMatrix(vec3 cPos, vec3 lPos) { // Custom Function
	mat4 camMatrix = mat4(
		1, 0, 0, -cPos.x,
		0, 1, 0, -cPos.y,
		0, 0, 1, cPos.z - LOOK_RADIUS,
		0, 0, 0, 1
	);

	mat3 rotMatrix = calcRotMatrix(lPos);

	mat4 camRotMatrix = mat4(
		rotMatrix[0][0], rotMatrix[0][1], rotMatrix[0][2], 0,
		rotMatrix[1][0], rotMatrix[1][1], rotMatrix[1][2], 0,
		rotMatrix[2][0], rotMatrix[2][1], rotMatrix[2][2], 0,
		0, 0, 0, 1
	);

	// return camMatrix;
	return camMatrix * camRotMatrix;
}

// Main

void main() {
	vec3 angles = calcRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x + offset.x, angles.y + offset.y, angles.z + (-offset.z + 0.5) * 2, 1.0f);

	mat4 cameraMatrix = calcCamMatrix(cam_pos, look_pos);
	gl_Position = final_pos * cameraMatrix * projMatrix;
	gl_Position -= vec4(0, 0, LOOK_RADIUS, 0); // move back to offset camera location

	vertexNo = gl_VertexID;
	pos_out = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
}