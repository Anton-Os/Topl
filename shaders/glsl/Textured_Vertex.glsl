#version 440

// Values

layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec3 rotation;
	vec3 texScroll; // texture coordinate scrolling
};

layout(std140, binding = 1) uniform SceneBlock{
	uint mode;
	vec3 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 texcoord;

layout(location = 0) out vec3 texcoord_out;
layout(location = 1) out uint mode_out;

// Functions

mat3 calcRotMatrix(vec3 angles) {
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

mat4 calcCamMatrix(vec3 cPos, vec3 lPos) { // placeholder camera
	mat4 camMatrix = mat4(
		1, 0, 0, -cPos.x,
		0, 1, 0, -cPos.y,
		0, 0, 1, -cPos.z,
		0, 0, 0, 1
	);

	return camMatrix;
}

// Main

void main() {
	vec3 angles = calcRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f);

	// gl_Position = (final_pos + vec4(offset, 0.0f)) * projMatrix;
	gl_Position = (final_pos + vec4(offset, 0.0f)) * calcCamMatrix(cam_pos, look_pos) * projMatrix;
	texcoord_out = texcoord + texScroll;
	mode_out = mode;
}