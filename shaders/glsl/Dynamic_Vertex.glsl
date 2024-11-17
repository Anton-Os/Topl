#version 440

#define INCLUDE_BLOCK

#include "Common.glsl"

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
	
	double timeElapse;
	// TODO: Include other dynamic parameters
};

layout(location = 0) out vec3 pos_out;
layout(location = 1) out vec4 vert_color_out;
layout(location = 2) flat out int id_out;

// Main

void main() {
	vec3 angles = getRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f) * vec4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	// gl_Position = (final_pos + vec4(offset, 0.0f)) * projMatrix;
	gl_Position = (final_pos + vec4(offset, 0.0f)) * getCamMatrix(cam_pos, look_pos) * projMatrix;

	pos_out = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
	id_out = gl_VertexID;
	vert_color_out = vec4(1.0, 1.0, 1.0, 1.0); // getRandColor(color - (color / (gl_VertexID + 1))); // getStepColor(gl_VertexID);
}