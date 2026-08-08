#version 440

#define IGNORE_INPUTS
// #define INCLUDE_EXTBLOCK

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
layout(location = 1) flat in uint near_index;
layout(location = 2) flat in uint second_index;
layout(location = 3) flat in uint far_index;
layout(location = 4) in vec3 vertex_pos;
layout(location = 5) in vec4 vertex_color;
layout(location = 6) in vec3 texcoord;

layout(location = 0) out vec4 color_final;

#include "field/Field.glsl"

// Main

void main() {
	vec3 target;
	if(mode >= 0) target = vertex_pos;
	else target = pos;

	uint m = abs(mode) / 10;
	uint target_idx = near_index;
	vec3 ctrlPoint = ctrlPoints[target_idx];

	/* if(abs(mode) >= 1000){
		vec3 change = vec3(sin(float(timeElapse) / 1000.0), cos(float(timeElapse) / 1000.0), tan(float(timeElapse) / 1000.0));

		if(floor(mode / 1000) == 1) target += change;
		else if(floor(mode / 1000) == 2) target -= change;
		else target *= change;
	} */

	// for(uint r = 0; r < (abs(mode) / 1000) + 1; r++){
	vec3 relCoord = ctrlPoint - target;
	uint indices[3] = { near_index, second_index, far_index };
	
	if(m % 10 == 1) target = field1(ctrlPoint, target);  
	else if(m % 10 == 2) target = field2(indices, target);
	else if(m % 10 == 3) target = field3(ctrlPoint, target);
	else if(m % 10 == 4) target = field4(target_idx, target);  
	else if(m % 10 == 5) target = field7(target_idx, target);
	else if(m % 10 == 6) target = field8(target_idx, target);
	else if(m % 10 == 7) target = field9(target_idx, target);
	else if(m % 10 == 8) target = field10(target_idx, target);
	else if(m % 10 == 9) target = field12(ctrlPoint, target, vec3(vertex_color));
	else target = vec3(length(relCoord), length(relCoord), length(relCoord));

	color_final = vec4(target.r - floor(target.r), target.g  - floor(target.g), target.b - floor(target.b), 1.0);
	// }
#ifdef INCLUDE_TEXTURES
	color_final *= modalTex(abs(mode / 100), texcoord);
#endif
	if (color_final.a < 0.05 || length(color_final.rgb) < 0.05) discard;
}
