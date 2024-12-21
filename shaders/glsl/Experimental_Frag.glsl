#version 440

#define IGNORE_INPUTS
#define INCLUDE_BLOCK
// #define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#include "Common.glsl"

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
	
	double timeFrame;
	double timeElapse;
	vec2 ctrlPoints[2];
	// vec3 lightVal;
	// vec3 lightPos;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) flat in int id;

layout(location = 0) out vec4 outColor;

// Main

void main() {
	float r = sin(float(timeElapse) / 1000.0F);
	float g = cos(float(timeElapse) / 1000.0F);
	float b = tan(float(timeFrame));

	vec2 nearestPoint = ctrlPoints[0];
	for(uint n = 1; n < 8; n++) 
		if(length(vec2(pos.x, pos.y) - ctrlPoints[n]) < length(vec2(pos.x, pos.y) - nearestPoint)) nearestPoint = ctrlPoints[n];
	
	if(mode >= 0) outColor = vec4(r * abs(nearestPoint.x), g * abs(nearestPoint.y), b * abs(pos.z), 1.0); // color shift mode
	// else perform texture calculations with light
}
