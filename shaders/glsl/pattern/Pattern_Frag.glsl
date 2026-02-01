#version 440

#define INCLUDE_TEXTURES

#include "Common.glsl"

#include "Pixel.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	// uint actorID;
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

#include "pattern/Pattern.glsl"

// Main

void main() {
	vec3 coords = pos;
	uint m = abs(mode) % 10; // uint(id);

	if(m == 1) coords = vec3(vertex_color);
	else if(m == 2) coords = normal;
	else if(m == 3) coords = tangent;
	else if(m == 4) coords = texcoord;
	else if(m == 5) coords = getRandColor(id);
	else if(m == 6) coords = vertex_pos / texcoord - normal * tangent;
	else if(m == 7) coords = vec3(sin(texcoord.x / pos.x), cos(normal.y / pos.y), tan(tangent.z / pos.z));
	else if(m == 8) coords = vec3(pow(abs(pos.x), abs(vertex_color.r)), pow(abs(pos.y), abs(texcoord.y)), pow(abs(pos.z), float(id)));
	else if(m == 9) coords = vec3(dot(vertex_pos, vec3(vertex_color)), dot(getRandColor(uint(id)), vec3(getStepColor(uint(id)))), dot(texcoord, tangent));

	if(mode < 0) m = uint(id);
	// double t = timeElapse / 5000.0;
	double t = sin(float(timeElapse) / 5000.0) * (timeElapse / 30000.0);

	if(abs(mode / 100) % 10 == 1) color_final = vec4(solidPattern1(coords, m + 1), 1.0);
	else if(abs(mode / 100) % 10 == 2) color_final = vec4(solidPattern2(coords, m + 1), 1.0);
	else if(abs(mode / 100) % 10 == 3) color_final = vec4(solidPattern3(coords, m + 1), 1.0);
	else if(abs(mode / 100) % 10 == 4) color_final = vec4(texturePattern1(coords, m, (abs(mode) % 100) / 5), 1.0);
	else if(abs(mode / 100) % 10 == 5) color_final = vec4(texturePattern2(coords, m, (abs(mode) % 100) / 5), 1.0);
	else if(abs(mode / 100) % 10 == 6) color_final = vec4(weavePattern1(coords), 1.0);
	else if(abs(mode / 100) % 10 == 7) color_final = vec4(weavePattern2(coords, uint((abs(mode) % 100) / 10)), 1.0);
	else if(abs(mode / 100) % 10 == 8) color_final = portalPattern1(coords, m, t);
	else if(abs(mode / 100) % 10 == 9) color_final = portalPattern2(coords, m, t);
	// else if(abs(mode / 100) % 10 == 8) color_final = vec4(texturePattern1(weavePattern1(coords), m, (mode % 100) / 5), 1.0);
	// else if(abs(mode / 100) % 10 == 9) color_final = vec4(weavePattern2(texturePattern2(coords, m, (mode % 100) / 5), uint((mode % 100) / 10)), 1.0);
	
	else color_final = vec4(coords, 1.0);

	color_final = vec4(
		abs(color_final.r) - floor(abs(color_final.r)), 
		abs(color_final.g) - floor(abs(color_final.g)), 
		abs(color_final.b) - floor(abs(color_final.b)), 
		color_final.a
	);
}
