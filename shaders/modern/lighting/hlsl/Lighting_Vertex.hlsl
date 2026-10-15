#include "Modern_Common.hlsl"

#include "Modern_Vertex.hlsl"

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	float3 worldPos = getWorldPos(input.pos.xyz);
	VS_OUTPUT output = passVaryings(input, worldPos, getModelNormal(input.pos.xyz));
	output.extra = float4(fpsView(worldPos).z, 0.0, 0.0, 0.0); // view depth
	return output;
}
