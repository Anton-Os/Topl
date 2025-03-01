#version 440

#define IGNORE_INPUTS
// #define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#define PATTERN_SIZE 0.025

#include "Common.glsl"

#include "Pixel.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec3 rotation;
	vec3 scale;

	mat4 ctrlMatrix;
	float alpha;
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

vec3 coordPattern(vec3 ctrlPoint, vec3 coords){
	float dist = length(ctrlPoint - coords);
	float size = abs(mode % 100) * PATTERN_SIZE;

	return vec3(dist * abs(ctrlPoint.r), dist  * abs(ctrlPoint.g), dist * abs(ctrlPoint.b)) * size;
}

vec3 trigPattern(vec3 ctrlPoint, vec3 coords, vec3 color){
	float dist = length(ctrlPoint - coords);
	float size = abs(mode % 100) * PATTERN_SIZE;

	ctrlPoint += vec3(sin(float(timeElapse) / 1000), cos(float(timeElapse) / 1000), tan(float(timeElapse) / 1000)) * size;
	// else ctrlPoint *= (float(timeElapse) / 1000) * size;

	ctrlPoint = vec3(sin(ctrlPoint.x * abs(mode % 100)), cos(ctrlPoint.y * abs(mode % 100)), tan(ctrlPoint.z * abs(mode % 100))) * dist;

	return ctrlPoint / color;
}

vec3 centerPattern(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	vec3 angles = vec3(atan(relCoord.y / relCoord.x), atan(relCoord.x / relCoord.z), atan(relCoord.z / relCoord.y));

	return vec3(angles.r - floor(angles.r), angles.g - floor(angles.g), angles.b - floor(angles.b)) * abs(mode % 100);
}

// Main

void main() {
	vec3 target;
	if(mode >= 0) target = vertex_pos;
	else target = pos;

	vec3 nearestPoint = ctrlPoints[ctrl_index];
	vec3 relCoord = nearestPoint - target;

	if(abs(mode) > 0 && abs(mode) < 100) color_final = vec4(coordPattern(nearestPoint, target), 1.0);  
	else if(abs(mode) >= 100 && abs(mode) < 200) color_final = vec4(trigPattern(nearestPoint, target, vec3(vertex_color)), 1.0);
	else if(abs(mode) >= 200 && abs(mode) < 300) color_final = vec4(centerPattern(nearestPoint, target), 1.0);  
	else color_final = vec4(abs(relCoord.x) - floor(abs(relCoord.x)), abs(relCoord.y) - floor(abs(relCoord.y)), abs(relCoord.z) - floor(abs(relCoord.z)), 1.0);
}
