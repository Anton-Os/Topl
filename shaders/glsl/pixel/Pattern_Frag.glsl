#version 440

#define INCLUDE_TEXTURES

#include "Common.glsl"

#include "Pixel.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	double timeFrame;
	double timeElapse;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vertex_pos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 vertex_color;
layout(location = 4) in vec3 texcoord;
layout(location = 5) in vec3 tangent;
layout(location = 6) flat in int id;

layout(location = 0) out vec4 color_final;

// Functions

/* vec3 tagPattern(int vertex_id, vec3 coords){
	if(vertex_id % 3 == 0) return vec3(sin(coords.x), cos(coords.y), tan(coords.z));
	else if(vertex_id % 3 == 1) return vec3(sin(-coords.x), cos(-coords.y), tan(-coords.z));
	else return coords * (1.0 / vertex_id);
} */

vec3 trialPattern(vec3 coords){
	uint m = uint(mode / 10) + 1;

	float r = ((coords.x * 5) - floor(coords.x * 5)) * m; // pow(coords.x, coords.y) * m;
	float g = ((coords.y * 10) - floor(coords.y * 10)) * m; // pow(coords.y, coords.z) * m;
	float b = ((coords.z * 50) - floor(coords.z * 50)) * m; // pow(coords.z, coords.x) * m;

	return vec3(r, g, b);
}

vec3 trialPattern2(vec3 coords){
	uint m = uint(mode / 10) + 1;

	float r = pow(abs(coords.x + coords.z), abs(coords.y)) * m;
	float g = pow(abs(coords.z - coords.y), abs(coords.x)) * m;
	float b = pow(abs(coords.y * coords.x), abs(coords.z)) * m;

	return vec3(r, g, b);
}

vec3 trialPattern3(vec3 coords){
	uint m = uint(mode / 10) + 1;

	float r = abs(sin(coords.x * 2) + sin(coords.y * 5) + sin(coords.z * 10)) * m;
	float g = abs(cos(coords.y * 2) * cos(coords.z * 5) * cos(coords.x * 10)) * m;
	float b = pow(pow(tan(coords.z * 2), tan(coords.z * 5)), tan(coords.y * 10)) * m;

	return vec3(r, g, b);
}

// Main

void main() {
	vec3 coords = pos;

	if(mode % 10 == 1) coords = vertex_pos;
	else if(mode % 10 == 2) coords = vec3(vertex_color.r, vertex_color.g, vertex_color.b);
	else if(mode % 10 == 3) coords = normal;
	else if(mode % 10 == 4) coords = tangent;
	else if(mode % 10 == 5) coords = texcoord;
	else if(mode % 10 == 6) coords = getRandColor(id);
	else if(mode % 10 == 7) coords = vertex_pos / texcoord - normal * tangent;
	else if(mode % 10 == 8) coords = vec3(pow(abs(pos.x), vertex_color.r), pow(abs(pos.y), vertex_pos.y), pow(abs(pos.z), float(id)));
	else if(mode % 10 == 9) coords = vec3(sin(texcoord.x * pos.x), cos(normal.y * pos.y), tan(tangent.z * pos.z));

	color_final = vec4(trialPattern3(coords), 1.0);
}
