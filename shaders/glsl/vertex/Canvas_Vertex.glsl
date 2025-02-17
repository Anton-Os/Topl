#version 440

#define INCLUDE_TEXTURES
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
	// double drawSize;
	vec2 tracerSteps[TRACER_STEPS];
	vec2 tracerPaths[TRACER_PATHS];
};

layout(location = 0) out vec3 texcoord_out;

// Main

void main() {
	vec4 pos = getVertex(pos_in, offset, rotation, vec4(scale, 1.0 / cam_pos.w));
	gl_Position = pos * getCamMatrix(cam_pos, look_pos) * projMatrix;

	texcoord_out = texcoord;
}