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

	return vec3(dist * abs(ctrlPoints[ctrl_index].r), dist  * abs(ctrlPoints[ctrl_index].g), dist * abs(ctrlPoints[ctrl_index].b)) * size;
}

vec3 trigPattern(vec3 ctrlPoint, vec3 coords, vec3 color){
	float dist = length(ctrlPoint - coords);
	float size = abs(mode % 100) * PATTERN_SIZE;

	ctrlPoint += vec3(sin(float(timeElapse) / 1000), cos(float(timeElapse) / 1000), tan(float(timeElapse) / 1000)) * size;
	// else ctrlPoint *= (float(timeElapse) / 1000) * size;

	ctrlPoint = vec3(sin(ctrlPoints[ctrl_index].x * abs(mode % 100)), cos(ctrlPoints[ctrl_index].y * abs(mode % 100)), tan(ctrlPoints[ctrl_index].z * abs(mode % 100))) * dist;

	return ctrlPoint / color;
}

vec3 centerPattern(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	vec3 angles = vec3(atan(relCoord.y / relCoord.x), atan(relCoord.x / relCoord.z), atan(relCoord.z / relCoord.y));

	return vec3(angles.r - floor(angles.r), angles.g - floor(angles.g), angles.b - floor(angles.b)) * abs(mode % 100);
}

vec3 proximaPattern(uint ctrlIdx, vec3 coords){
	vec3 relCoord = ctrlPoints[ctrlIdx] - coords;
	for(uint c = 0; c < 8; c++) 
		if(c != ctrlIdx){
			vec4 point = vec4(ctrlPoints[c], 1.0) * ctrlMatrix;
			relCoord += (vec3(abs(point.x), abs(point.y), abs(point.z)) - coords) * (1.0 / (100 - abs(mode % 100)));
		} 

	return relCoord;
}

vec3 neonPattern(uint ctrlIdx, vec3 coords){
	vec3 relCoord = ctrlPoints[ctrlIdx] - coords;
	float r = abs(relCoord.x) / abs(relCoord.y);
	float g = abs(relCoord.x) / abs(relCoord.z);
	float b = abs(relCoord.z) / abs(relCoord.y);

	return relCoord * vec3(r, g, b) * (1.0 / (100 - abs(mode % 100)));
}

vec3 crypticPattern(uint ctrlIdx, vec3 coords){
	vec3 coord1 = ctrlPoints[ctrlIdx] - coords;
	vec3 coord2 = ctrlPoints[(ctrlIdx + 1) % 8] - coords;
	vec3 coord3 = ctrlPoints[abs(ctrlIdx - 1)] - coords;

	return vec3(distance(coord1, coord2), distance(coord2, coord3), distance(coord1, coord3));
}

vec3 farPattern(uint ctrlIdx, vec3 coords){
	vec3 nearestPoint = ctrlPoints[ctrlIdx] - coords;
	vec3 farthestPoint = ctrlPoints[0] - coords;
	for(uint c = 1; c < 8; c++) 
		if(c != ctrlIdx && distance(ctrlPoints[c], nearestPoint) > distance(farthestPoint, nearestPoint))
			farthestPoint = ctrlPoints[c];

	return vec3(abs(farthestPoint.x - nearestPoint.x), abs(farthestPoint.y - nearestPoint.y), abs(farthestPoint.z - nearestPoint.z));
}

vec3 timePattern(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	float timeSeq = float((timeElapse / 2500) - floor(timeElapse / 2500)) * 5;

	return vec3(abs(sin(relCoord.x * timeSeq)), abs(cos(relCoord.y * timeSeq)), abs(tan(relCoord.z * timeSeq)));
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
	else if(abs(mode) >= 300 && abs(mode) < 400) color_final = vec4(proximaPattern(ctrl_index, target), 1.0);  
	else if(abs(mode) >= 400 && abs(mode) < 500) color_final = vec4(neonPattern(ctrl_index, target), 1.0);
	else if(abs(mode) >= 500 && abs(mode) < 600) color_final = vec4(crypticPattern(ctrl_index, target), 1.0);
	else if(abs(mode) >= 600 && abs(mode) < 700) color_final = vec4(farPattern(ctrl_index, target), 1.0); 
	else if(abs(mode) >= 700 && abs(mode) < 800) color_final = vec4(timePattern(relCoord, target), 1.0); 
	else if(abs(mode) >= 800 && abs(mode) < 900) color_final = vec4(sin(ctrlPoints[ctrl_index].x - target.y), cos(abs(ctrlPoints[ctrl_index].y * target.x)), tan(pow(ctrlPoints[ctrl_index].z, target.z)), 1.0);
	// else color_final = vec4(abs(nearestPoint.x - target.x), abs(nearestPoint.y - target.y), abs(nearestPoint.z - target.z), 1.0);
	else color_final = vec4(abs(nearestPoint.x - target.x) * sin(float(timeElapse) / 1000), abs(nearestPoint.y - target.y) * cos(float(timeElapse) / 2500), abs(nearestPoint.z - target.z) * tan(float(timeElapse) / 3300), 1.0);
	// else color_final = vec4(abs(relCoord.x) - floor(abs(relCoord.x)), abs(relCoord.y) - floor(abs(relCoord.y)), abs(relCoord.z) - floor(abs(relCoord.z)), 1.0);
}
