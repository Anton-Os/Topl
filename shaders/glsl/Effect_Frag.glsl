#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

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

vec3 cursorSet(vec2 cursorPos, vec2 coord){
	float red = pow(1 - min(abs(cursorPos.x - coord.x), abs(cursorPos.y - coord.y)), 20); // crosshairs
	float green = tan(distance(cursorPos, coord) * 50); // spirals
	float blue = 1 / distance(cursorPos, coord); // gradient
	return vec3(red, green, blue);
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

vec3 bubbleSet(vec2 coord1, vec2 coord2){
	uint i = 0; // iteration count

	vec3 target = vec3(0.5, 0.5, 0.5);
	while(((1.0 / coord1.x) * (1.0 / coord1.y) > pow(coord2.x * coord2.y, 1.0) || target == vec3(0.5, 0.5, 0.5)) && i < 100){
		coord1.x += coord2.y * i;
		coord1.y += coord2.x * i;
		target = vec3(1.0 / coord1.x, 1.0 / coord1.y, pow(coord1.x / coord1.y, coord2.x * coord2.y));
		i++;
	}

	return target;
}

vec3 trialSet1(vec2 coord){
	float centerDist = sqrt(pow((0.5 - coord.x), 2) + pow((0.5 - coord.y), 2));

	float a = sin(centerDist) * (3.0 * cos(coord.x / coord.y));
	float b = cos(centerDist) / (1.0 / tan(pow(coord.x * coord.y, 2.0)));
	float c = tan(centerDist) - abs(coord.x - coord.y) + pow(coord.y, coord.x);

	return vec3(a * (1.0 / cos(b)), b * sin(c), c * tan(a + b));
	// return vec3(pow(a, b), pow(b, c), pow(c, a));
}

vec3 trialSet2(vec2 coord1, vec2 coord2){
	uint i = 0;

	vec3 target = vec3(coord1.x * coord2.x, coord1.y * coord2.y, sqrt(coord1.x + coord1.y) - sqrt(coord2.x - coord2.y));
	while((target.r > target.g || target.g > target.b || target.b > target.r) && i < 100){
		if(target.r > target.g) target.r *= 1.0 / coord1.x * coord1.y;
		else if(target.g > target.b) target.g *= coord2.y / coord2.x;
		else if(target.b > target.r) target.b *= coord1.x / coord2.y;
		i++;
	}

	return target;
}

vec3 trialSet3(vec2 coord){
	float divX = (coord.x * 10) - floor(coord.x * 10);
	float divY = (coord.y * 10) - floor(coord.y * 10);

	return vec3(divX / divY, pow(divX, divY), divX + divY);
}

vec3 trialSet4(vec2 coord1, vec2 coord2){
	uint i = 0;

	vec3 target = vec3(1.0, 1.0, 1.0);
	while(i < 10){
		coord1.x = abs(1.0 - coord2.y);
		coord1.y = abs(1.0 - coord2.x);
		coord2.x += abs(coord1.y * coord2.y);
		coord2.y += abs(coord1.x * coord2.x);
		target += vec3(coord1.x, coord1.y, coord2.x + coord2.y);
		i++;
	}

	return normalize(target);
}


// Main

void main() {
	vec2 cursor = (tracerSteps[0] * 0.5f) + 0.5f; // adjusted cursor
	vec2 coords = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates

	if (mode == 1) color = vec4(cursorSet(cursor, coords), 1.0f); // cursor track mode
	else if(mode == 2) color = vec4(bubbleSet(tracerSteps[0] - tracerSteps[1], coords), 1.0f); // test set
	else if(mode == 3) color = vec4(trialSet1(coords - cursor), 1.0);
	else if(mode == 4) color = vec4(trialSet2(tracerSteps[0], coords - cursor), 1.0);
	else if(mode == 5) color = vec4(trialSet3(coords - cursor), 1.0);
	else if(mode == 6) color = vec4(trialSet4(tracerSteps[0], coords - cursor), 1.0);
	else color = vec4(mandlebrotSet((coords - cursor) * FRACTAL_SIZE), 1.0f); // fractal mode
}
