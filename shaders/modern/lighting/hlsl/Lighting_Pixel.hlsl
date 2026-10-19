#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

// Main
// actor_params: x shininess, y reflectivity, z emissive strength, w smooth sphere normals

float4 main(PS_INPUT input) : SV_TARGET {
	uint m = getModes(mode).x % 4u;
	float3 n = getSurfaceNormal(input, m == 1u);
	float3 v = normalize(cam_pos.xyz - input.world_pos);
	float4 albedo = getActorColor();
	albedo.rgb = applyTexture(albedo.rgb, input);

	float shininess = (actor_params.x > 0.0)? actor_params.x : 48.0;
	float reflectivity = saturate(actor_params.y);
	float emissive = max(actor_params.z, 0.0);

	float3 color = shadeBlinnPhong(albedo.rgb, n, input.world_pos, shininess, 0.6);
	float3 reflection = getSkyColor(reflect(-v, n));
	color = lerp(color, reflection, getFresnel(n, v, 5.0) * (0.15 + reflectivity * 0.85));
	color += albedo.rgb * emissive;

	if(m == 2u) color = n * 0.5 + 0.5; // normal view
	else if(m == 3u) color = (1.0 - saturate(input.extra.x / (max(cam_lens.w, 1.0) * 0.25))).xxx; // depth view
	else color = applyFog(color, input.world_pos);

	float alpha = applyTextureAlpha(albedo.a, input);
	if(alpha < 0.04) discard; // transparent sprite pixels
	return float4((m >= 2u)? color : toneMap(color), alpha);
}
