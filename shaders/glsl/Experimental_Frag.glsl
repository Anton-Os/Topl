#version 440

#define IGNORE_INPUTS
// #define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#include "Common.glsl"

// Values

layout(std140, binding = 0) uniform Block {
        vec3 offset;
        vec3 rotation;
        vec3 scale;

        mat4 ctrlMatrix;
};

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
	
	double timeFrame;
	double timeElapse;
	vec3 ctrlPoints[8];
	// vec3 lightVal;
	// vec3 lightPos;
};

layout(location = 0) in vec3 pos;
layout(location = 1) flat in uint ctrl_index;
layout(location = 2) in vec4 vertex_color;

layout(location = 0) out vec4 outColor;

// Main

void main() {
	vec3 nearestPoint = ctrlPoints[ctrl_index];
	float nearestDist = length(nearestPoint - pos);

	float r = sin(float(timeElapse * vertex_color.r) / 1000.0F);
	float g = cos(float(timeElapse * vertex_color.g) / 1000.0F);
	float b = tan(float(timeFrame * vertex_color.b));
	
	if(mode >= 0) outColor = vec4(r * nearestDist, g * nearestDist, b * nearestDist, 1.0); // color shift mode
	// else perform texture calculations with light
}
