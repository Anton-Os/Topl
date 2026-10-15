#include "Modern_Common.hlsl"

#include "Modern_Vertex.hlsl"

// Hologram: horizontal slices of the mesh glitch sideways at random intervals

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	float3 worldPos = getWorldPos(input.pos.xyz);

	float t = getTime();
	float slice = floor(worldPos.y * 9.0);
	float tick = floor(t * 14.0);
	float chance = hash12(float2(slice, tick));
	float strength = (actor_params.x > 0.0)? actor_params.x : 1.0;
	float glitch = (chance > 0.93)? (hash12(float2(tick, slice + 7.0)) - 0.5) * 0.35 * strength : 0.0;
	if(getModes(mode).x % 4u == 2u) glitch = 0.0; // stable mode

	worldPos.x += glitch;
	worldPos.z += glitch * 0.5;

	VS_OUTPUT output = passVaryings(input, worldPos, getModelNormal(input.pos.xyz));
	output.extra = float4(glitch, slice, 0.0, 0.0);
	return output;
}
