#version 440

#define STAGE_VERTEX

#include "Modern_Common.glsl"

#include "Modern_Vertex.glsl"

// Object picking: actors are drawn through the FPS camera in a flat color that encodes their id

void main() {
	vec3 worldPos = getWorldPos(pos_in);
	gl_Position = fpsTransform(worldPos);
	passVaryings(worldPos, vec3(0.0));
}
