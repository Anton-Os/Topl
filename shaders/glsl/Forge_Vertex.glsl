#version 440

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

layout(location = 0) out vec3 pos_out;
layout(location = 1) flat out uint ctrl_index_out;
layout(location = 2) out vec4 vert_color_out;

uint calcCtrlPointIndex(vec3 target){
	uint index = 0;
	for(uint n = 1; n < 8; n++){ 
		vec3 tformTarget = vec3(vec4(target, 1.0) * ctrlMatrix);
		if(length(tformTarget - ctrlPoints[n]) < length(tformTarget - ctrlPoints[index])) index = n;
	}
	return index;
}

void main() {
	vec3 angles = getRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f) * vec4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);
	// final_pos = final_pos * vec4(1.0f + cos(float(timeElapse)), 1.0f + cos(float(timeElapse)), 1.0f + cos(float(timeElapse)), 1.0f);

	// gl_Position = (final_pos + vec4(offset, 0.0f)) * projMatrix;
	gl_Position = (final_pos + vec4(offset, 0.0f)) * getCamMatrix(cam_pos, look_pos) * projMatrix;

	pos_out = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
	ctrl_index_out = calcCtrlPointIndex(pos_out);
	vert_color_out = vec4(vert_color, 0.5); // getStepColor(ctrl_index_out);
}