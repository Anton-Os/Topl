#version 440

#define IGNORE_INPUTS
#define INCLUDE_SCENEBLOCK
// #define INCLUDE_EXTBLOCK

#define FLAT_ID 0
#define FLAT_DIRECTIONAL 1
#define FLAT_COORD 2
#define FLAT_VERTEX 3
#define FLAT_CAMERA 4
#define FLAT_ANGULAR 5
#define FLAT_TEXCOORD 6
#define FLAT_NORMAL 7
#define FLAT_SECTIONED 8
#define FLAT_RANDOM 9
// #define FLAT_TRIAL 9

#include "Common.glsl"

#include "Pixel.glsl"

// Values

layout(std140, binding = 0) uniform Block{
	vec4 color;
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) flat in int id;
layout(location = 3) in vec3 texcoord;
layout(location = 4) in vec3 normal;

layout(location = 0) out vec4 color_final;

// Main

void main() {
	uint primID = gl_PrimitiveID;
	
	float alpha = 1.0f;
	// if(abs(mode) >= 10) alpha -= -pos.z * ceil(mode / 10.0);

	if(mode >= 10) color_final = vertex_color;
	else if(mode == -1) color_final = getStepColor(id); // indexing mode
	else if(mode < -1){
		uint target = primID;
		while(target > mode * -1) target -= mode * -1;
		color_final = getStepColor(target);
	}
	else if(mode % 10 == FLAT_DIRECTIONAL) color_final = vec4((pos.x / 2.0) + 0.5, (pos.y / 2.0) + 0.5, (pos.z / 2.0) + 0.5, alpha);
	else if(mode % 10 == FLAT_COORD) color_final = vec4((pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (pos.z - offset.z) * 2.0 + 0.5, color.a);
	else if(mode % 10 == FLAT_VERTEX) color_final = vec4(vertex_color.r, vertex_color.g, vertex_color.b, alpha);
	else if(mode % 10 == FLAT_CAMERA) color_final = vec4(abs(cam_pos.x - pos.x), abs(cam_pos.y - pos.y), abs(cam_pos.z - pos.z), alpha);
	else if(mode % 10 == FLAT_ANGULAR) color_final = vec4(atan((pos.y - offset.y) / (pos.x - offset.x)), atan((pos.y - offset.y) / (pos.z - offset.z)), atan((pos.x - offset.x) / (pos.z - offset.z)), alpha);
	else if(mode % 10 == FLAT_TEXCOORD) color_final = vec4(texcoord.x, texcoord.y, texcoord.z, alpha);
	else if(mode % 10 == FLAT_NORMAL) color_final = vec4(normal.x, normal.y, normal.z, alpha);
	else if(mode % 10 == FLAT_SECTIONED) color_final = vec4(((id + 1) / 3.0) - floor((id + 1) / 3.0), ((id + 1) / 6.0) - floor((id + 1) / 6.0), ((id + 1) / 9.0) - floor((id + 1) / 9.0), alpha);
	// else if(mode % 10 == FLAT_SECTIONED) color_final = vec4(abs(gl_FragCoord.x * 80.0) - abs(floor(gl_FragCoord.x * 80.0)), abs(gl_FragCoord.y * 80.0) - abs(floor(gl_FragCoord.y * 80.0)), (gl_FragCoord.z * 8.0) - floor(gl_FragCoord.z * 8.0), alpha);
	else if(mode % 10 == FLAT_RANDOM) color_final = vec4(getRandColor(uint(floor((color.r + color.b + color.g) * 546.32))), alpha);
	// else if(mode % 10 == FLAT_TRIAL) color_final = vec4(sin(vertex_color.r * id), cos(vertex_color.g * primID), tan(vertex_color.b * (id / primID)), alpha);
	else color_final = color; // solid mode // default
}
