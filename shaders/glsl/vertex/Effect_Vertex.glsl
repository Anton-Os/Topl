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
	
	ivec2 screenRes;
	vec2 cursorPos;
	float effectSize;
	uint effectIters;
};

layout(location = 1) out vec3 texcoord_out;

// Main

void main() {
	vec4 pos = getVertex(pos_in, offset, rotation, vec4(scale, 1.0 / cam_pos.w));
	gl_Position = pos * getCamMatrix(cam_pos, look_pos) * projMatrix;

	texcoord_out = texcoord_in;
}
