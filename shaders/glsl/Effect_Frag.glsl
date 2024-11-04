#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

#define FRACTAL_SIZE 3.0 // max fractal size
#define FRACTAL_ITER 100 // max fractal iteratons
#define C vec2(cursorPos.x, cursorPos.y) // c value for julia set

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
	
	ivec2 screenRes; // resolution
	vec2 cursorPos;
};

layout(location = 0) out vec4 color;

// Functions

// Mandlebrot Set
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

// Julia set
vec3 juliaSet(vec2 coord){
	uint i = 0; // iteration count

	while (dot(coord, coord) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		double x = (coord.x * coord.x) - (coord.y * coord.y);
		double y = 2.0 * coord.x * coord.y;
		coord = vec2(x, y) + C;
		i++;
	}

	if (i < FRACTAL_ITER) return vec3(1.0f / i, tan(i), 0.05f * i); // custom colors outside set
	return vec3(0, 0, 0); // black color within set
}


// Main

void main() {
	vec2 cursor = (cursorPos * 0.5f) + 0.5f; // adjusted cursor
	vec2 coords = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates

	if(mode >= 10 && mode < 20) color = vec4(juliaSet((coords - cursor) * FRACTAL_SIZE), 1.0f);
	else color = vec4(mandlebrotSet((coords - cursor) * FRACTAL_SIZE), 1.0f); // fractal mode
}
