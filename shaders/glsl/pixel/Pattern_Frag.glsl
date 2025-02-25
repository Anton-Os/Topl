#version 440

#define IGNORE_INPUTS
// #define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#define PATTERN_SIZE 0.1

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
};

layout(location = 0) in vec3 pos;
layout(location = 1) flat in uint ctrl_index;
layout(location = 2) in vec3 vertex_pos;
layout(location = 3) in vec4 vertex_color;

layout(location = 0) out vec4 color_final;

// Functions

vec3 coordPattern(vec3 ctrlPoint, vec3 position){
	float dist = length(ctrlPoint - position);
	float size = abs(mode) * PATTERN_SIZE;

	return vec3(dist * abs(ctrlPoint.r), dist  * abs(ctrlPoint.g), dist * abs(ctrlPoint.b)) * size;
}

vec3 flashPattern(vec3 ctrlPoint, vec3 position, vec3 color){
	float dist = length(ctrlPoint - position);
	float size = abs(mode) * PATTERN_SIZE;

	if(mode > 0){
		ctrlPoint.x += sin(float(timeElapse) / 1000) * size;
		ctrlPoint.y += cos(float(timeElapse) / 1000) * size;
		ctrlPoint.z += tan(float(timeElapse) / 1000) * size;
	} else ctrlPoint *= (float(timeElapse) / 1000) * size;

	// TODO: Accomodate different modes
	float r = sin(ctrlPoint.x * abs(mode % 10)) * dist;
	float g = cos(ctrlPoint.y * abs(mode % 10)) * dist;
	float b = tan(ctrlPoint.z * abs(mode % 10)) * dist;

	return vec3(r, g, b) / color;
}

// Main

void main() {
	vec3 nearestPoint = ctrlPoints[ctrl_index];

	color_final = vec4(coordPattern(nearestPoint, vertex_pos), 1.0);
	// color_final = vec4(flashPattern(nearestPoint, vertex_pos, vec3(vertex_color)), 1.0);
}
