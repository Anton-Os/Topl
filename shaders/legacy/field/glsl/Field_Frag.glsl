#version 440

#define IGNORE_INPUTS
// #define INCLUDE_MESHBLOCK

#define FIELD_SIZE 0.025

#include "Common.glsl"

#include "Frag.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	// uint actorID;
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
layout(location = 1) flat in uint near_index;
layout(location = 2) flat in uint second_index;
layout(location = 3) flat in uint far_index;
layout(location = 4) in vec3 vertex_pos;
layout(location = 5) in vec4 vertex_color;
layout(location = 6) in vec3 texcoord;

layout(location = 0) out vec4 color_final;

#include "Field.glsl"

vec3 getCtrlPtsSum(){
	vec3 sum = vec3(0.0, 0.0, 0.0);
	for(uint n = 0; n < 16; n++) sum += ctrlPoints[n];
	return sum;
}

float getCtrlPtsLengths(){
	float lengthTotal = 0.0;
	for(uint n = 0; n < 16; n++) lengthTotal += length(ctrlPoints[n]);
	return lengthTotal;
}

// Main

void main() {
	vec3 target;
	if(mode >= 0) target = vertex_pos;
	else target = pos;
	vec3 nearPt = ctrlPoints[near_index];
	vec3 secPt = ctrlPoints[second_index];
	vec3 farPt = ctrlPoints[far_index];

	// Control point selection

	vec3 ctrlPoint = nearPt;// - secPt - farPt;
	switch(abs(mode) % 10){
		case 1: ctrlPoint = farPt; break;
		case 2: ctrlPoint = secPt; break;
		case 3: ctrlPoint = getCtrlPtsSum(); break;
		case 4: ctrlPoint *= getCtrlPtsLengths(); break;
		case 5: ctrlPoint = nearPt * secPt * farPt; break;
		case 6: ctrlPoint = nearPt - secPt - farPt; break;
		case 7: ctrlPoint = nearPt + secPt - farPt; break;
		case 8: ctrlPoint = nearPt - secPt * farPt; break;
		case 9: ctrlPoint = smoothstep(nearPt, secPt, farPt); break;
		default: ctrlPoint = nearPt; break;
	}

	// Algorithm selection

	uint m = abs(mode) / 10;
	vec3 relCoord = ctrlPoint - target;
	if(m % 10 == 1) target = dist_field1(ctrlPoint, target);  
	else if(m % 10 == 2) target = dist_field2(ctrlPoint, target);
	else if(m % 10 == 3) target = dist_field3(ctrlPoint, target);
	else if(m % 10 == 4) target = cross_field1(ctrlPoint, target);  
	else if(m % 10 == 5) target = cross_field2(ctrlPoint, target);
	else if(m % 10 == 6) target = cross_field3(ctrlPoint, target);
	else if(m % 10 == 7) target = trig_field1(ctrlPoint, secPt, farPt, target);
	else if(m % 10 == 8) target = trig_field2(ctrlPoint, secPt, farPt, target);
	else if(m % 10 == 9) target = trig_field3(ctrlPoint, secPt, farPt, target);
	else target = vec3(length(relCoord) / 2, length(relCoord) / 2, length(relCoord) / 2);

	color_final = vec4(target.r - floor(target.r), target.g  - floor(target.g), target.b - floor(target.b), 1.0);
#ifdef INCLUDE_TEXTURES
	color_final *= modalTex(abs(mode / 1000), texcoord);
#endif
	if (color_final.a < 0.05 || length(color_final.rgb) < 0.05) discard;
}
