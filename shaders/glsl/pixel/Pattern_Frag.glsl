#version 440

#define INCLUDE_TEXTURES

#include "Common.glsl"

#include "Pixel.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	double timeFrame;
	double timeElapse;
};

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 vertex_pos;
layout(location = 3) in vec4 vertex_color;
layout(location = 4) in vec3 tangent;

layout(location = 0) out vec4 color_final;

// Functions

// TODO: Add Patterns

// Main

void main() {
	color_final = vec4(tangent - pos, 1.0);
}
