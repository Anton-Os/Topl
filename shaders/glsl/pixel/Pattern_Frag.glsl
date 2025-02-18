#version 440

#define IGNORE_INPUTS
// #define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#define PATTERN_SIZE 0.5

#include "Common.glsl"

#include "Pixel.glsl"

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
	float size = abs(mode) * PATTERN_SIZE;

	if(mode > 0){
		nearestPoint.x += sin(float(timeElapse) / 1000) * size;
		nearestPoint.y += cos(float(timeElapse) / 1000) * size;
		nearestPoint.z += tan(float(timeFrame) / 1000) * size;
	} else nearestPoint *= (float(timeElapse) / 1000) * size;

	float r = sin(nearestPoint.x * abs(mode % 10)) * nearestDist;
	float g = cos(nearestPoint.y * abs(mode % 10)) * nearestDist;
	float b = tan(nearestPoint.z * abs(mode % 10)) * nearestDist;
	
	// if(mode >= 0) 
	outColor = vec4(r, g, b, 1.0) / vertex_color;
	// else outColor = vec4(r, g, b, 1.0) * vertex_color;
}
