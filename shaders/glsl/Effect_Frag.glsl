#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

#define CURSOR_SIZE 0.05
#define FRACTAL_SIZE 3.0 // max fractal size
#define FRACTAL_ITER 100 // max fractal iteratons

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

layout(location = 0) out vec4 color;

// Functions

/* vec3 cursorSet(vec2 cursorPos, vec2 coord){
	float red = pow(1 - min(abs(cursorPos.x - coord.x), abs(cursorPos.y - coord.y)), 20); // crosshairs
	float green = tan(distance(cursorPos, coord) * 50); // spirals
	float blue = 1 / distance(cursorPos, coord); // gradient
	return vec3(red, green, blue);
} */

vec4 cursorDot(vec2 pos, vec2 coord, float radius, vec4 color){
    if (distance(pos, coord) < radius) return color;
    else return vec4(color.r, color.g, color.b, color.a * 0.1); // nearly transparent
}

vec4 cursorHalo(vec2 pos, vec2 coord, float radius, vec4 color){
    if (distance(pos, coord) > radius * 0.75 && distance(pos, coord) < radius * 1.25) return color;
    else return vec4(color.r, color.g, color.b, color.a * 0.1); // nearly transparent
}

vec4 cursorCross(vec2 pos, vec2 coord, float radius, vec4 color){
    if((abs(coord.x - pos.x) < radius * 0.5 && abs(coord.y - pos.y) < radius * 0.1)
        || (abs(coord.y - pos.y) < radius * 0.5 && abs(coord.x - pos.x) < radius * 0.1))
        return color;
    else return vec4(color.r, color.g, color.b, color.a * 0.1); // nearly transparent
}

// mandlebrotSet Set
vec3 mandlebrotSet(vec2 coord){
	uint i = 0; // iteration count
	double x = 0; double y = 0;

	while(x * x + y * y <= FRACTAL_SIZE && i < FRACTAL_ITER){
		double temp = (x * x) - (y * y) + coord.x;
		y = (2 * x * y) + coord.y;
		x = temp;
		i++;
	}

	if(i < FRACTAL_ITER) return vec3(0.05f * i, sin(i), 1.0f / i); // custom colors outside set
	else return vec3(0.0f, 0.0f, 0.0f); // black color within set
}

// Main

void main() {
	vec2 cursor = (tracerSteps[0] * 0.5f) + 0.5f; // adjusted cursor
	vec2 coords = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates

	if(mode < 0) color = vec4(mandlebrotSet((coords - cursor) * FRACTAL_SIZE), 1.0f); // fractal mode
	else if(mode == 1) color = cursorHalo(cursor, coords, CURSOR_SIZE, vec4(1.0, 1.0, 1.0, 0.75));
    else if(mode == 2) color = cursorCross(cursor, coords, CURSOR_SIZE, vec4(1.0, 1.0, 1.0, 0.75));
    else color = cursorDot(cursor, coords, CURSOR_SIZE, vec4(1.0, 1.0, 1.0, 0.75));
}
