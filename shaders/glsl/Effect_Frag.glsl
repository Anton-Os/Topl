#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

#define FRACTAL_SIZE 3.0 // max fractal size
#define FRACTAL_ITER 1000 // max fractal iteratons

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

vec3 cursorTarget(vec2 cursorPos, vec2 coord){
	float red = pow(1 - min(abs(cursorPos.x - coord.x), abs(cursorPos.y - coord.y)), 20); // crosshairs
	float green = tan(distance(cursorPos, coord) * 50); // spirals
	float blue = 1 / distance(cursorPos, coord); // gradient
	return vec3(red, green, blue);
}

// mandlebrotSet Set
vec3 mandlebrotSet(vec2 coord){
	double x = 0; double y = 0;
	uint i = 0; // iteration count

	while(x * x + y * y <= FRACTAL_SIZE && i < FRACTAL_ITER){
		double temp = (x * x) - (y * y) + coord.x;
		y = (2 * x * y) + coord.y;
		x = temp;
		i++;
	}

	if(i < FRACTAL_ITER) return vec3(0.05f * i, sin(i), 1.0f / i); // custom colors outside set
	else return vec3(0.0f, 0.0f, 0.0f); // black color within set
}

vec3 trialSet(vec2 coord1, vec2 coord2){
	uint i = 0; // iteration count

	vec3 target = vec3(0.5, 0.5, 0.5);
	while((1.0 / coord1.x) * (1.0 / coord1.y) > abs(coord2.x * coord2.y) && i < 100){
		coord1.x += coord2.y * i;
		coord1.y += coord2.x * i;
		target = vec3(1.0 / coord1.x, 1.0 / coord1.y, coord1.x * coord2.y);
		i++;
	}

	return target;
}

// Main

void main() {
	vec2 cursorPosAdj = (tracerSteps[0] * 0.5f) + 0.5f; // adjusted cursor
	vec2 coordsAdj = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates

	if (mode == 1) color = vec4(cursorTarget(cursorPosAdj, coordsAdj), 1.0f); // cursor track mode
	else if(mode == 2) color = vec4(trialSet(tracerSteps[0], coordsAdj), 1.0f); // test set
	else color = vec4(mandlebrotSet((coordsAdj - cursorPosAdj) * FRACTAL_SIZE), 1.0f); // fractal mode
}
