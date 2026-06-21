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

#ifdef INCLUDE_TEXTURES
	// color_final *= modalTex(abs(mode / 100), texcoord);
#endif
}
