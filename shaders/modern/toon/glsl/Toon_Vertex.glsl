#version 440

#define STAGE_VERTEX

#include "Modern_Common.glsl"

#include "Modern_Vertex.glsl"

// Main

void main() {
	vec3 worldPos = getWorldPos(pos_in);
	gl_Position = fpsTransform(worldPos);
	passVaryings(worldPos, getModelNormal(pos_in));
	extra = vec4(fpsView(worldPos).z, 0.0, 0.0, 0.0); // view depth
}
