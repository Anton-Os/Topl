#include "Modern_Common.hlsl"

#include "Modern_Vertex.hlsl"

#include "Terrain.hlsl"

// Main: grid vertices are lifted by the procedural height field in world space

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	float3 worldPos = getWorldPos(input.pos.xyz);
	float height = getTerrainHeight(worldPos.xz);
	float water = getWaterLevel();

	worldPos.y = offset.y + max(height, water);
	if(height < water) worldPos.y += sin(worldPos.x * 0.8 + getTime() * 1.5) * cos(worldPos.z * 0.6 + getTime()) * 0.05; // gentle swell

	VS_OUTPUT output = passVaryings(input, worldPos, float3(0.0, 0.0, 0.0));
	output.extra = float4(height, water, 0.0, 0.0);
	return output;
}
