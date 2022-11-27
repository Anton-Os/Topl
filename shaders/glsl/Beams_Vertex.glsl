#version 440

// Values

layout(std140, binding = 0) uniform Block {
	// uint renderID
	vec3 offset;
	vec3 rotation;
};

layout(std140, binding = 1) uniform SceneBlock {
	uint mode;
	vec3 cam_pos;
	vec3 look_pos;
	// mat4 projMatrix;

	vec3 skyLight_pos; vec3 skyLight_value;
	vec3 flashLight_pos; vec3 flashLight_value;
	vec3 lampLight_pos; vec3 lampLight_value;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 1) out vec3 pos_out;

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

	return camMatrix;;
}

// Main

void main() {
	vec3 angles = calcRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f);

	gl_Position = final_pos + vec4(offset, 0.0f);
	pos_out = vec3(final_pos.x, final_pos.y, final_pos.z);
}