#version 440

#define INCLUDE_INPUTS
#define INCLUDE_SCENEBLOCK

#include "Common.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	vec4 color;
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(location = 0) out vec3 pos_out;
layout(location = 1) flat out int id_out;
layout(location = 2) out vec4 color_out;

// Main

void main() {
	vec3 angles = getRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f) * vec4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	// gl_Position = (final_pos + vec4(offset, 0.0f)) * projMatrix;
	gl_Position = (final_pos + vec4(offset, 0.0f)) * getCamMatrix(cam_pos, look_pos) * projMatrix;

	pos_out = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
	id_out = gl_VertexID;
	color_out = color - (color / (gl_VertexID + 1)); // getUniqueColor(gl_VertexID);
}