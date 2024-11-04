#version 440

#define IGNORE_INPUTS
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

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) flat in int id;

layout(location = 0) out vec4 outColor;

// Main

void main() {
	float r = sin(float(timeElapse) / 1000.0F);
	float g = cos(float(timeElapse) / 1000.0F);
	float b = tan(float(timeElapse) / 1000.0F);
	
	outColor = vec4(r, g, b, 1.0); // solid mode // default
}
