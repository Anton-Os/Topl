#version 440

#define INCLUDE_BLOCK

#include "Common.glsl"

#include "Vertex.glsl"

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	vec3 skyLight[2]; // position and value
	vec3 flashLight[2]; // position and value
	vec3 lampLight[2]; // position and value
};

layout(location = 0) out vec3 pos_out;
layout(location = 1) out vec3 vertex_pos_out;
layout(location = 2) out vec3 normal_out;

// Main

void main() {
	vec3 angles = getRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f) * vec4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	vertex_pos_out = vec3(final_pos.x, final_pos.y, final_pos.z);
	normal_out = getRotMatrix(rotation) * normal;

	gl_Position = (final_pos + vec4(offset, 0.0f)) * getCamMatrix(cam_pos, look_pos) * projMatrix;
	pos_out = vec3(final_pos.x, final_pos.y, final_pos.z);
}
