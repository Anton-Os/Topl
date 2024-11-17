#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS
#define INCLUDE_TEXTURES

#include "Common.glsl"

#define CURSOR_SIZE 0.05

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
	
	ivec2 screenRes; // resolution
	vec2 cursorPos;
	vec2 tracerSteps[8];
	vec2 tracerPaths[8];
};

layout(location = 0) in vec3 texcoord;

layout(location = 0) out vec4 color_out;

// Functions

/* vec3 cursorSet(vec2 cursorPos, vec2 coord){
	float red = pow(1 - min(abs(cursorPos.x - coord.x), abs(cursorPos.y - coord.y)), 20); // crosshairs
	float green = tan(distance(cursorPos, coord) * 50); // spirals
	float blue = 1 / distance(cursorPos, coord); // gradient
	return vec3(red, green, blue);
} */

vec4 cursorDot(vec2 pos, vec2 coord, float radius, vec4 color){
    if (distance(pos, coord) < radius) return color;
    else return vec4(color.r, color.g, color.b, 0.0); // nearly transparent
}

vec4 cursorHalo(vec2 pos, vec2 coord, float radius, vec4 color){
    if (distance(pos, coord) > radius * 0.75 && distance(pos, coord) < radius * 1.25) return color;
    else return vec4(color.r, color.g, color.b, 0.0); // nearly transparent
}

vec4 cursorCross(vec2 pos, vec2 coord, float radius, vec4 color){
    if((abs(coord.x - pos.x) < radius * 0.5 && abs(coord.y - pos.y) < radius * 0.1) || (abs(coord.y - pos.y) < radius * 0.5 && abs(coord.x - pos.x) < radius * 0.1))
        return color;
    else return vec4(color.r, color.g, color.b, 0.0); // nearly transparent
}

// Main

void main() {
	if(mode >= 0) color_out = color_correct(texture(baseTex, vec2(texcoord.x, texcoord.y))); // base texture

	vec2 cursor = (cursorPos * 0.5f) + 0.5f; // adjusted cursor
	vec2 coords = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates

	if(abs(mode) >= 10){
		vec4 color_cursor = vec4(0.0, 0.0, 0.0, 0.0);
		if(uint(floor(abs(mode) / 10.0)) % 10 == 1) color_cursor = cursorHalo(cursor, coords, CURSOR_SIZE, vec4(1.0, 1.0, 1.0, 0.75));
		else if(uint(floor(abs(mode) / 10.0)) % 10 == 2) color_cursor = cursorCross(cursor, coords, CURSOR_SIZE, vec4(1.0, 1.0, 1.0, 0.75));
		else color_cursor = cursorDot(cursor, coords, CURSOR_SIZE, vec4(1.0, 1.0, 1.0, 0.75));

		if(color_cursor.a != 0.0) color_out = color_cursor;
	}
}
