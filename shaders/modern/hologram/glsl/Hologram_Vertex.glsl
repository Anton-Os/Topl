#version 440

#define STAGE_VERTEX

#include "Modern_Common.glsl"

#include "Modern_Vertex.glsl"

// Hologram: horizontal slices of the mesh glitch sideways at random intervals

void main() {
	vec3 worldPos = getWorldPos(pos_in);

	float t = getTime();
	float slice = floor(worldPos.y * 9.0);
	float tick = floor(t * 14.0);
	float chance = hash12(vec2(slice, tick));
	float strength = (actor_params.x > 0.0)? actor_params.x : 1.0;
	float glitch = (chance > 0.93)? (hash12(vec2(tick, slice + 7.0)) - 0.5) * 0.35 * strength : 0.0;
	if(getModes(mode).x % 4u == 2u) glitch = 0.0; // stable mode

	worldPos.x += glitch;
	worldPos.z += glitch * 0.5;

	gl_Position = fpsTransform(worldPos);
	passVaryings(worldPos, getModelNormal(pos_in));
	extra = vec4(glitch, slice, 0.0, 0.0);
}
