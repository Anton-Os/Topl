#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

#include "Pixel.glsl"

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

#define FRACTAL_SIZE effectSize
#define FRACTAL_ITER (effectIters * 5)

layout(location = 1) in vec3 texcoord;

layout(location = 0) out vec4 color_final;

// Functions

vec3 fractalColors(vec2 coord, vec2 cursor, uint i){
	float dist = pow(distance(coord, cursor), float(FRACTAL_SIZE - i));

	if(abs(mode) < 100){
		if(mode % 10 == 0) return vec3(1.0f / i, tan(i), 0.05f * i);
		else if(mode % 10 == 1) return vec3(getRandColor(i).r, getRandColor(i).g, getRandColor(i).b);
		else if(mode % 10 == 2) return vec3(pow(coord.x, i), pow(coord.y, 1.0 / i), pow(coord.x, coord.y));
		else if(mode % 10 == 3) return vec3(dist - floor(dist), ceil(dist) - dist, pow(dist, dist));
		else if(mode % 10 == 4) return vec3(coord.x * i - floor(coord.y * i), ceil(coord.y * i) - coord.x * i, cursor.x * cursor.y * i - floor(cursor.x * cursor.y * i));
		else if(mode % 10 == 5) return vec3(i / (FRACTAL_ITER * 0.5), (coord.x + coord.y) / FRACTAL_SIZE, ((coord.x - cursor.x) * (coord.y - cursor.y)) / FRACTAL_SIZE);
		else if(mode % 10 == 6) return vec3(dot(coord, cursor), length(coord - cursor), smoothstep(0.0, 1.0, pow(coord.x - cursor.x, coord.y - cursor.y))) * getRandColor(i);
		else if(mode % 10 == 7) return vec3(sin(1.0f / coord.x) + cos(1.0f / coord.y), atan(cursor.x / cursor.y), pow(abs(cursor.x + cursor.y), abs(coord.x * coord.y)));
		else if(mode % 10 == 8) return vec3(sin(dot(coord, cursor)), cos(dot(-coord, cursor)), tan(sin(coord.x - cursor.x) / cos(coord.y - cursor.y)));
		else if(mode % 10 == 9) return vec3((coord - cursor) * i, dot(coord, cursor) / i); // TODO: Change this!
		// else if(mode % 10 == 9) return getCoordDistances(coord, cursor, vec2(0.5f, 0.5f)) * vec3(mod(sinh((coord.x - cursor.x) * i), 1.0), mod(cosh((coord.y - cursor.y) * i), 1.0), mod(tanh((coord.x - cursor.x) * (coord.y - cursor.y) * i), 1.0));
		// else if(mode % 10 == 9) return vec3(abs(noise2(coord - cursor).x), abs(noise2(cursor - coord).y), noise1(float(i) * length(coord - cursor)));
		else return vec3(coord.x, coord.y, 1.0 / i);
	} else {
		float r = (float(i) / FRACTAL_ITER) * dist;
		float g = (float(i) / FRACTAL_ITER) * (coord.x / coord.y);
		float b = (float(i) / FRACTAL_ITER) * dot(coord, cursor);

		return vec3(r, g, b) * floor(abs(mode) / 100);
	}
	// TODO: Include more color options
}

#include "effect/Effect.glsl"

// Main

void main() {
	uint m = abs(mode) % 100;
	vec2 cursor = (cursorPos * 0.5f) + 0.5f; // adjusted cursor
	vec2 coords = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates
	float size = cam_pos.w * FRACTAL_SIZE;

	vec2 target;
	if(mode >= 0) target = coords - cursor - vec2(cam_pos.x, cam_pos.y);
	else target = vec2(texcoord.x, texcoord.y) - cursor;

	if(m >= 10 && m < 20) color_final = vec4(juliaSet(target * size, cursor), 1.0f);
	else if(m >= 20 && m < 30) color_final = vec4(trigSet((target + vec2(0.5, 0.5)) * size), 1.0f);
	else if(m >= 30 && m < 40) color_final = vec4(powerSet(target * size, cursorPos), 1.0f);
	else if(m >= 40 && m < 50) color_final = vec4(wingSet(target), 1.0f);
	else if(m >= 50 && m < 60) color_final = vec4(stepSet(target, cursorPos), 1.0f);
	else if(m >= 60 && m < 70) color_final = vec4(loopSet(target), 1.0F);
	else if(m >= 70 && m < 80) color_final = vec4(shardSet(target, cursorPos), 1.0f);
	else if(m >= 80 && m < 90) color_final = vec4(sparseSet(target, abs((target.x - cursor.x) - (target.y - cursor.y))), 1.0f);
	else if(m >= 90 && m < 100) color_final = vec4(retroSet(target, cursor), 1.0f);
	// else if(m >= 100) color_final = vec4(recursiveAlgo(trigSet(target), wingSet(target), mandlebrotSet(target)), 1.0);
	// else if(m >= 100) color_final = vec4(julia(target * size, cursor, 3.0)/*1.0 + (float(m) / 100.0f)) */, 1.0);
	else color_final = vec4(mandlebrotSet(target * size), 1.0f); // fractal mode

	if(color_final.r == 0.0f && color_final.g == 0.0f && color_final.b == 0.0f) color_final.a = 0.0; // make transparent if not in set
}
