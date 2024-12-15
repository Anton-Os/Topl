#version 440

#define IGNORE_INPUTS
#define INCLUDE_SCENEBLOCK

#define FLAT_ID 0
#define FLAT_DIRECTIONAL 1
#define FLAT_COORD 2
#define FLAT_VERTEX 3
#define FLAT_CAMERA 4
#define FLAT_ANGULAR 5
#define FLAT_TEXCOORD 6
#define FLAT_SECTIONED 7
#define FLAT_RANDOM 8
#define FLAT_TRIAL 9

#include "Common.glsl"

// Values

layout(std140, binding = 0) uniform Block{
	vec4 color;
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) flat in int id;
layout(location = 3) in vec3 texcoord;

layout(location = 0) out vec4 outColor;

// Main

void main() {
	uint primID = gl_PrimitiveID;
	
	float alpha = 1.0f;
	if(abs(mode) >= 10) alpha -= -pos.z * ceil(mode / 10.0);

	if(mode == -1) // indexing mode
		outColor = getStepColor(id);
	else if(mode < -1){
		uint target = primID;
		while(target > mode * -1) target -= mode * -1;
		outColor = getStepColor(target);
	}
	else if(mode % 10 == FLAT_DIRECTIONAL) outColor = vec4((pos.x / 2.0) + 0.5, (pos.y / 2.0) + 0.5, (pos.z / 2.0) + 0.5, alpha);
	else if(mode % 10 == FLAT_COORD) outColor = vec4((pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (pos.z - offset.z) * 2.0 + 0.5, color.a);
	else if(mode % 10 == FLAT_VERTEX) outColor = vec4(vertex_color, alpha);
	else if(mode % 10 == FLAT_CAMERA) outColor = vec4(abs(cam_pos.x - pos.x), abs(cam_pos.y - pos.y), abs(cam_pos.z - pos.z), alpha);
	else if(mode % 10 == FLAT_ANGULAR) outColor = vec4(atan((pos.y - offset.y) / (pos.x - offset.x)), atan((pos.y - offset.y) / (pos.z - offset.z)), atan((pos.x - offset.x) / (pos.z - offset.z)), alpha);
	else if(mode % 10 == FLAT_TEXCOORD) outColor = vec4(texcoord.x, texcoord.y, texcoord.z, alpha);
	else if(mode % 10 == FLAT_SECTIONED) outColor = vec4(((id + 1) / 3.0) - floor((id + 1) / 3.0), ((id + 1) / 6.0) - floor((id + 1) / 6.0), ((id + 1) / 9.0) - floor((id + 1) / 9.0), alpha);
	// else if(mode % 10 == FLAT_SECTIONED) outColor = vec4(abs(gl_FragCoord.x * 80.0) - abs(floor(gl_FragCoord.x * 80.0)), abs(gl_FragCoord.y * 80.0) - abs(floor(gl_FragCoord.y * 80.0)), (gl_FragCoord.z * 8.0) - floor(gl_FragCoord.z * 8.0), alpha);
	else if(mode % 10 == FLAT_RANDOM) outColor = vec4(getRandColor(uint(floor((color.r + color.b + color.g) * 546.32))), alpha);
	else if(mode % 10 == FLAT_TRIAL) outColor = vec4(sin(vertex_color.r * id), cos(vertex_color.g * primID), tan(vertex_color.b * (id / primID)), alpha);
	else outColor = color; // solid mode // default
}
