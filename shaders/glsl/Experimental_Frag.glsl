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
layout(location = 1) in vec4 vertex_color;
layout(location = 2) flat in int id;

layout(location = 0) out vec4 outColor;

vec3 calcNearestPoint(vec3 target){
	vec3 nearestPoint = ctrlPoints[0];
	for(uint n = 1; n < 8; n++) 
		if(length(target - ctrlPoints[n]) < length(target - nearestPoint)) nearestPoint = ctrlPoints[n];
	return nearestPoint;
}

// Main

void main() {
	vec3 nearestPoint = calcNearestPoint(pos);

	float r = sin(float(timeElapse * nearestPoint.x) / 1000.0F);
	float g = cos(float(timeElapse * nearestPoint.y) / 1000.0F);
	float b = tan(float(timeFrame * nearestPoint.z));
	
	if(mode >= 0) outColor = vec4(r, g, b, 1.0); // color shift mode
	// else perform texture calculations with light
}
