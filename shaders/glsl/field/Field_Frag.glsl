#version 440

#define IGNORE_INPUTS
// #define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#define FIELD_SIZE 0.025

#include "Common.glsl"

#include "Pixel.glsl"

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
layout(location = 1) flat in uint ctrl_index;
layout(location = 2) in vec3 vertex_pos;
layout(location = 3) in vec4 vertex_color;
layout(location = 4) in vec3 tangent;

layout(location = 0) out vec4 color_final;

#include "field/Field.glsl"

// Main

void main() {
	vec3 target;
	if(mode >= 0) target = vertex_pos;
	else target = tangent;

	uint m = abs(mode) % 1000;
	vec3 nearestPoint = ctrlPoints[ctrl_index];

	/* if(abs(mode) >= 1000){
		vec3 change = vec3(sin(float(timeElapse) / 1000.0), cos(float(timeElapse) / 1000.0), tan(float(timeElapse) / 1000.0));

		if(floor(mode / 1000) == 1) target += change;
		else if(floor(mode / 1000) == 2) target -= change;
		else target *= change;
	} */

	for(uint r = 0; r < (abs(mode) / 1000) + 1; r++){
		vec3 relCoord = nearestPoint - target;
		uint indices[3] = { ctrl_index, (ctrl_index - 1) % 8, (ctrl_index + 1) % 8 };
		
		if(m > 0 && m < 100) target = field1(nearestPoint, target);  
		else if(m >= 100 && m < 200) target = field2(nearestPoint, target, vec3(vertex_color));
		else if(m >= 200 && m < 300) target = field3(nearestPoint, target);
		else if(m >= 300 && m < 400) target = field4(ctrl_index, target);  
		else if(m >= 400 && m < 500) target = field9(ctrl_index, target);
		else if(m >= 500 && m < 600) target = field8(ctrl_index, target);
		else if(m >= 600 && m < 700) target = field12(indices, target);
		else if(m >= 700 && m < 800) target = field10(ctrl_index, target);
		else if(m >= 800 && m < 900) target = field7(ctrl_index, target);
		else target = vec3(length(relCoord), length(relCoord), length(relCoord));

		color_final = vec4(target.r - floor(target.r), target.g  - floor(target.g), target.b - floor(target.b), 1.0);
	}
}
