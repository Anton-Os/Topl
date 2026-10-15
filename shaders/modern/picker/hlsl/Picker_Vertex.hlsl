#include "Modern_Common.hlsl"

#include "Modern_Vertex.hlsl"

// Object picking: actors are drawn through the FPS camera in a flat color that encodes their id

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	float3 worldPos = getWorldPos(input.pos.xyz);
	return passVaryings(input, worldPos, float3(0.0, 0.0, 0.0));
}
