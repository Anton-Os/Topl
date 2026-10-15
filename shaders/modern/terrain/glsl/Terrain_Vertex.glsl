#version 440

#define STAGE_VERTEX

#include "Modern_Common.glsl"

#include "Modern_Vertex.glsl"

#include "Terrain.glsl"

// Main: grid vertices are lifted by the procedural height field in world space

void main() {
	vec3 worldPos = getWorldPos(pos_in);
	float height = getTerrainHeight(worldPos.xz);
	float water = getWaterLevel();

	worldPos.y = offset.y + max(height, water);
	if(height < water) worldPos.y += sin(worldPos.x * 0.8 + getTime() * 1.5) * cos(worldPos.z * 0.6 + getTime()) * 0.05; // gentle swell

	gl_Position = fpsTransform(worldPos);
	passVaryings(worldPos, vec3(0.0));
	extra = vec4(height, water, 0.0, 0.0);
}
