#include "Modern_Common.hlsl"

#include "Modern_Vertex.hlsl"

#include "Spacetime.hlsl"

// Main: a flat grid is pulled down by every mass to visualize curvature

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	float3 worldPos = getWorldPos(input.pos.xyz);
	float well = getWell(worldPos.xz);
	float ripple = getRipple(worldPos.xz);
	worldPos.y = offset.y - well + ripple;

	VS_OUTPUT output = passVaryings(input, worldPos, float3(0.0, 0.0, 0.0));
	output.extra = float4(well, ripple, 0.0, 0.0);
	return output;
}
