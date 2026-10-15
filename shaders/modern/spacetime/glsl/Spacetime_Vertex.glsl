#version 440

#define STAGE_VERTEX

#include "Modern_Common.glsl"

#include "Modern_Vertex.glsl"

#include "Spacetime.glsl"

// Main: a flat grid is pulled down by every mass to visualize curvature

void main() {
	vec3 worldPos = getWorldPos(pos_in);
	float well = getWell(worldPos.xz);
	float ripple = getRipple(worldPos.xz);
	worldPos.y = offset.y - well + ripple;

	gl_Position = fpsTransform(worldPos);
	passVaryings(worldPos, vec3(0.0));
	extra = vec4(well, ripple, 0.0, 0.0);
}
