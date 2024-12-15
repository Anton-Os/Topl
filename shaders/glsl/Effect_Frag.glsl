#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
	
	ivec2 screenRes; // resolution
	vec2 cursorPos;
	float effectSize;
	uint effectIters;
};

#define FRACTAL_SIZE 5.0
#define FRACTAL_ITER 20 // max fractal iteratons

layout(location = 1) in vec3 texcoord;

layout(location = 0) out vec4 color;

// Functions

vec3 fractalColors(vec2 coord, vec2 cursor, uint i){
	if(mode % 10 == 0) return vec3(1.0f / i, tan(i), 0.05f * i);
	else if(mode % 10 == 1) return vec3(getRandColor(i).r, getRandColor(i).g, getRandColor(i).b);
	else if(mode % 10 == 2) return vec3(pow(coord.x, i), pow(coord.y, 1.0 / i), pow(coord.x, coord.y));
	else if(mode % 10 == 3) return vec3(distance(coord, cursor), distance(coord, vec2(0.5, 0.5)), distance(cursor, vec2(0.5, 0.5)));
	else if(mode % 10 == 4) return vec3(coord.x * i - floor(coord.y * i), ceil(coord.y * i) - coord.x * i, cursor.x * cursor.y * i - floor(cursor.x * cursor.y * i));
	else if(mode % 10 == 5) return vec3(i / (FRACTAL_ITER * 0.5), (coord.x + coord.y) / FRACTAL_SIZE, ((coord.x - cursor.x) * (coord.y - cursor.y)) / FRACTAL_SIZE);
	else if(mode % 10 == 6) return getRandColor(i) * vec3(dot(coord, cursor), length(coord - cursor), smoothstep(0.0, 1.0, pow(coord.x - cursor.x, coord.y - cursor.y)));
	else if(mode % 10 == 7) return vec3(sin(1.0f / coord.x) + cos(1.0f / coord.y), atan(cursor.x / cursor.y), pow(abs(cursor.x + cursor.y), abs(coord.x * coord.y)));
	else if(mode % 10 == 8) return vec3(sin(dot(coord, cursor)), cos(dot(-coord, cursor)), tan(sin(coord.x - cursor.x) / cos(coord.y - cursor.y)));
	else if(mode % 10 == 9) return getCoordDistances(coord, cursor, vec2(0.5f, 0.5f)) * vec3(mod(sinh((coord.x - cursor.x) * i), 1.0), mod(cosh((coord.y - cursor.y) * i), 1.0), mod(tanh((coord.x - cursor.x) * (coord.y - cursor.y) * i), 1.0));
	// else if(mode % 10 == 8) return vec3(modf(sinh((coord.x - cursor.x) * i), 1.0f), modf(cosh((coord.y - cursor.y) * i), 1.0f), modf(tanh(length(coord - cursor) * i), 1.0f));
	// else if(mode % 10 == 9) return vec3(abs(noise2(coord - cursor).x), abs(noise2(cursor - coord).y), noise1(float(i) * length(coord - cursor)));
	else return vec3(coord.x, coord.y, 1.0 / i);
	// TODO: Include more color options
}

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

	if(i < FRACTAL_ITER) return fractalColors(vec2(x, y), cursorPos, i);
	else return vec3(0.0f, 0.0f, 0.0f); // black color within set
}

// Julia set
vec3 juliaSet(vec2 coord, vec2 cursor){
	uint i = 0; // iteration count

	while (dot(coord, coord) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		double x = (coord.x * coord.x) - (coord.y * coord.y);
		double y = 2.0 * coord.x * coord.y;
		coord = vec2(x, y) + cursor;
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

// Trig Set
vec3 trigSet(vec2 coord){
	uint i = 0; // iteration count

	while(abs(sin(coord.x) + cos(coord.y)) * abs(1.0 / (coord.x * coord.y)) < FRACTAL_SIZE && i < FRACTAL_ITER){
		double x = coord.x * sin(coord.y); // switch operations
		double y = coord.y * cos(coord.x); // switch operations
		coord = vec2(x, y);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursorPos, i);
	return vec3(0, 0, 0); // black color within set
}

// Power set
vec3 powerSet(vec2 coord, vec2 cursor){
	uint i = 0; // iteration count

	// while(pow(pow(abs(coord.x + coord.y), abs(coord.x - coord.y)), (float(i) + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	while(pow(pow(abs(coord.x / coord.y), abs(coord.y / coord.x)), (float(i) + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	// while(pow(pow(log(coord.x / coord.y), log(coord.y / coord.x)), (float(i) + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
		float x = abs(coord.x);
		float y = abs(coord.y);
		coord = vec2(x + pow(x, y) / cursor.x, y + pow(y, x) / cursor.y);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

// Wing Set
vec3 wingSet(vec2 coord){
    uint i = 1;
    float x = coord.x; float y = coord.y;

    while(abs(x / coord.x) * abs(y / coord.y) < FRACTAL_SIZE && i < FRACTAL_ITER){
        // x += smoothstep(coord.x + x, coord.x - x, 1.0 / (i * i)) * pow(coord.x, x / y); // sin(y * i);
        // y += smoothstep(coord.y - y, coord.y + y, 1.0 / sqrt(i)) * pow(coord.y, y / x); // cos(x * i);
        x *= smoothstep(coord.x, coord.x + abs(x), i * i) * pow(coord.x / abs(y), coord.x * i); // experiment with abs() for effects
        y *= smoothstep(coord.y, coord.y + abs(y), sqrt(i)) * pow(coord.y / abs(x), coord.y * i); // experiment with abs() for effects
        i++;
    }

    if (i < FRACTAL_ITER) return fractalColors(vec2(x, y), cursorPos, i);
    return vec3(0, 0, 0);
}

// Step Set
vec3 stepSet(vec2 coord, vec2 cursor){
	uint i = 1; // iteration count

	while(((coord.y * (1.0 / coord.x)) - floor(coord.y * (1.0 / coord.x))) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		// coord = vec2((coord.x * i) - floor(coord.x * i), ceil(coord.y * i) - (coord.y * i));
		coord *= pow(i, dot(coord, cursor)) / pow(distance(coord, cursor), i);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

// Main

void main() {
	vec2 cursor = (cursorPos * 0.5f) + 0.5f; // adjusted cursor
	vec2 coords = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates
	float size = cam_pos.w * FRACTAL_SIZE;

	vec2 target;
	if(mode >= 0) target = coords - cursor - vec2(cam_pos.x, cam_pos.y);
	else target = vec2(texcoord.x - 0.5, texcoord.y - 0.5) - cursor;

	if(abs(mode) >= 10 && abs(mode) < 20) color = vec4(juliaSet(target * size, cursor), 1.0f);
	else if(abs(mode) >= 20 && abs(mode) < 30) color = vec4(trigSet((target + vec2(0.5, 0.5)) * size), 1.0f);
	else if(abs(mode) >= 30 && abs(mode) < 40) color = vec4(powerSet(target * size, cursorPos), 1.0f);
        else if(abs(mode) >= 40 && abs(mode) < 50) color = vec4(wingSet(target), 1.0f);
        else if(abs(mode) >= 50 && abs(mode) < 60) color = vec4(stepSet(target, cursorPos), 1.0f);
	else color = vec4(mandlebrotSet(target * size), 1.0f); // fractal mode

	if(color.r == 0.0f && color.g == 0.0f && color.b == 0.0f) color.a = 0.0; // make transparent if not in set
}
