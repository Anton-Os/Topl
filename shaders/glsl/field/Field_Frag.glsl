#version 440

#define IGNORE_INPUTS
// #define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#define FIELD_SIZE 0.025

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
	vec3 ctrlPoints[16];
};

layout(location = 0) in vec3 pos;
layout(location = 1) flat in uint ctrl_index;
layout(location = 2) in vec3 vertex_pos;
layout(location = 3) in vec4 vertex_color;
layout(location = 4) in vec3 tangent;

layout(location = 0) out vec4 color_final;

// Functions

vec3 coordField(vec3 ctrlPoint, vec3 coords){
	float dist = length(ctrlPoint - coords);
	float size = abs(mode % 100) * FIELD_SIZE;

	return vec3(dist * abs(ctrlPoints[ctrl_index].r), dist * abs(ctrlPoints[ctrl_index].g), dist * abs(ctrlPoints[ctrl_index].b)) * size;
}

vec3 trigField(vec3 ctrlPoint, vec3 coords, vec3 color){
	float dist = length(ctrlPoint - coords);
	float size = abs(mode % 100) * FIELD_SIZE;

	ctrlPoint += vec3(sin(float(timeElapse) / 1000), cos(float(timeElapse) / 1000), tan(float(timeElapse) / 1000)) * size;
	// else ctrlPoint *= (float(timeElapse) / 1000) * size;

	ctrlPoint = vec3(sin(ctrlPoints[ctrl_index].x * abs(mode % 100)), cos(ctrlPoints[ctrl_index].y * abs(mode % 100)), tan(ctrlPoints[ctrl_index].z * abs(mode % 100))) * dist;

	return ctrlPoint / color;
}

vec3 centerField(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	vec3 angles = vec3(atan(relCoord.y / relCoord.x), atan(relCoord.x / relCoord.z), atan(relCoord.z / relCoord.y));

	return vec3(angles.r - floor(angles.r), angles.g - floor(angles.g), angles.b - floor(angles.b)) * abs(mode % 100);
}

vec3 proximaField(uint ctrlIdx, vec3 coords){
	vec3 relCoord = ctrlPoints[ctrlIdx] - coords;
	for(uint c = 0; c < 8; c++) 
		if(c != ctrlIdx){
			vec4 point = vec4(ctrlPoints[c], 1.0) * ctrlMatrix;
			relCoord += (vec3(abs(point.x), abs(point.y), abs(point.z)) - coords) * (1.0 / (100 - abs(mode % 100)));
		} 

	return relCoord;
}

vec3 neonField1(uint ctrlIdx, vec3 coords){
	vec3 relCoord = ctrlPoints[ctrlIdx] - coords;
	float r = abs(relCoord.x) / abs(relCoord.y);
	float g = abs(relCoord.x) / abs(relCoord.z);
	float b = abs(relCoord.z) / abs(relCoord.y);

	return relCoord * vec3(r, g, b) * (1.0 / (100 - abs(mode % 100)));
}

vec3 neonField2(uint ctrlIdx, vec3 coords){
	vec3 relCoord = ctrlPoints[ctrlIdx] - coords;
	float r = pow(abs(relCoord.x), abs(coords.x));
	float g = pow(abs(relCoord.y), abs(coords.y));
	float b = pow(abs(relCoord.z), abs(coords.z));

	return relCoord * vec3(r, g, b) * (1.0 / (100 - abs(mode % 100)));
}

vec3 neonField3(uint ctrlIdx, vec3 coords){
	vec3 relCoord = ctrlPoints[ctrlIdx] - coords;
	float r = abs(tan(relCoord.x * length(relCoord * ctrlIdx)));
	float g = abs(tan(relCoord.y * length(relCoord * ctrlIdx)));
	float b = abs(tan(relCoord.z * length(relCoord * ctrlIdx)));

	return relCoord * vec3(r, g, b) * (1.0 / (100 - abs(mode % 100)));
}

vec3 crypticField(uint ctrlIdx, vec3 coords){
	vec3 coord1 = ctrlPoints[ctrlIdx] - coords;
	vec3 coord2 = ctrlPoints[(ctrlIdx + 1) % 8] - coords;
	vec3 coord3 = ctrlPoints[abs(ctrlIdx - 1)] - coords;

	uint m = (mode % 100) + 1;
	return vec3(pow(distance(coord1, coords), m), pow(distance(coord2, coords), m), pow(distance(coord3, coords), m));
}

vec3 alikeField(uint ctrlIdx, vec3 coords){
	vec3 secondPoint = vec3(10000.0, 10000.0, 10000.0);
	for(uint c = 0; c < 8; c++)
		if(c != ctrlIdx && distance(coords, ctrlPoints[c]) < distance(coords, secondPoint))
			secondPoint = ctrlPoints[c] - coords;
	
	uint m = (mode % 100) + 1;
	if(length(ctrlPoints[ctrlIdx] - secondPoint) > 0.01 * m) return (ctrlPoints[ctrlIdx] - secondPoint) * m;
	else return (secondPoint - ctrlPoints[ctrlIdx]) * m;
}

vec3 farField(uint ctrlIdx, vec3 coords){
	vec3 nearestPoint = ctrlPoints[ctrlIdx] - coords;
	vec3 farthestPoint = ctrlPoints[0] - coords;
	for(uint c = 1; c < 8; c++) 
		if(c != ctrlIdx && distance(coords, nearestPoint) > distance(coords, farthestPoint))
			farthestPoint = ctrlPoints[c];

	uint m = (mode % 100) + 1;
	return vec3(abs(farthestPoint.x - nearestPoint.x), abs(farthestPoint.y - nearestPoint.y), abs(farthestPoint.z - nearestPoint.z)) * m;
}

vec3 timeField(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	float timeSeq = float((timeElapse / 2500) - floor(timeElapse / 2500)) * ((mode % 100) + 1);

	return vec3(abs(sin(relCoord.x * timeSeq)), abs(cos(relCoord.y * timeSeq)), abs(tan(relCoord.z * timeSeq)));
}

vec3 triField(uint ctrlIndices[3], vec3 coords){
	return vec3(
		length(ctrlPoints[ctrlIndices[0]] - coords),
		length(ctrlPoints[ctrlIndices[1]] - coords),
		length(ctrlPoints[ctrlIndices[2]] - coords)
	);
}

// Main

void main() {
	vec3 target;
	if(mode >= 0) target = vertex_pos;
	else target = tangent;

	uint m = abs(mode) % 1000;
	vec3 nearestPoint = ctrlPoints[ctrl_index];
	vec3 relCoord = nearestPoint - target;

	if(abs(mode) >= 1000){
		vec3 change = vec3(sin(float(timeElapse) / 1000.0), cos(float(timeElapse) / 1000.0), tan(float(timeElapse) / 1000.0));

		if(floor(mode / 1000) == 1) target += change;
		else if(floor(mode / 1000) == 2) target -= change;
		else target *= change;
	}

	uint indices[3] = { ctrl_index, (ctrl_index - 1) % 8, (ctrl_index + 1) % 8 };

	if(m > 0 && m < 100) color_final = vec4(coordField(nearestPoint, target), 1.0);  
	else if(m >= 100 && m < 200) color_final = vec4(trigField(nearestPoint, target, vec3(vertex_color)), 1.0);
	else if(m >= 200 && m < 300) color_final = vec4(centerField(nearestPoint, target), 1.0);
	else if(m >= 300 && m < 400) color_final = vec4(proximaField(ctrl_index, target), 1.0);  
	else if(m >= 400 && m < 500) color_final = vec4(alikeField(ctrl_index, target), 1.0);
	else if(m >= 500 && m < 600) color_final = vec4(crypticField(ctrl_index, target), 1.0);
	else if(m >= 600 && m < 700) color_final = vec4(triField(indices, target), 1.0);
	else if(m >= 700 && m < 800) color_final = vec4(farField(ctrl_index, target), 1.0);
	else if(m >= 800 && m < 900) color_final = vec4(neonField3(ctrl_index, target), 1.0);
	// else if(m >= 700 && m < 800) color_final = vec4(timeField(relCoord, target), 1.0); 
	// else if(m >= 800 && m < 900) color_final = vec4(sin(ctrlPoints[ctrl_index].x - target.y / target.y), cos(abs(ctrlPoints[ctrl_index].y * target.x / target.y)), tan(pow(ctrlPoints[ctrl_index].z, tan(target.z * 10))), 1.0);
	else color_final = vec4(length(relCoord), length(relCoord), length(relCoord), 1.0);
	// else color_final = vec4(getRandColor(ctrl_index * (gl_PrimitiveID % 8)), 1.0); // vec4(abs(nearestPoint.x - target.x) * sin(float(timeElapse) / 1000), abs(nearestPoint.y - target.y) * cos(float(timeElapse) / 2500), abs(nearestPoint.z - target.z) * tan(float(timeElapse) / 3300), 1.0);
	// else color_final = vec4(abs(relCoord.x) - floor(abs(relCoord.x)), abs(relCoord.y) - floor(abs(relCoord.y)), abs(relCoord.z) - floor(abs(relCoord.z)), 1.0);
}
