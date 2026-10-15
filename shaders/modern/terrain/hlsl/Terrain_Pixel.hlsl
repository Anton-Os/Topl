#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

// Terrain: height and slope based materials, reflective water, distance fog
// mode 0 natural, mode 1 topographic contours, mode 2 wire grid, mode 3 height map

float4 main(PS_INPUT input) : SV_TARGET {
	uint m = getModes(mode).x % 4u;
	float3 p = input.world_pos;
	float height = input.extra.x;
	float water = input.extra.y;
	float3 n = getFacetNormal(p);
	float3 v = normalize(cam_pos.xyz - p);
	float slope = 1.0 - n.y;
	float rel = height - water;

	float3 grass = lerp(float3(0.18, 0.36, 0.12), float3(0.32, 0.45, 0.16), valueNoise(p.xz * 0.35));
	float3 rock = lerp(float3(0.33, 0.3, 0.27), float3(0.45, 0.42, 0.38), valueNoise(p.xz * 1.3));
	float3 sand = float3(0.76, 0.68, 0.5);
	float3 snow = float3(0.92, 0.94, 0.98);

	float3 albedo = lerp(sand, grass, smoothstep(0.1, 0.6, rel));
	albedo = lerp(albedo, rock, smoothstep(0.25, 0.5, slope));
	albedo = lerp(albedo, snow, smoothstep(4.5, 6.0, rel + valueNoise(p.xz * 0.5) * 1.5) * (1.0 - smoothstep(0.45, 0.7, slope)));
	albedo = applyTexture(albedo, input);

	float3 color = shadeBlinnPhong(albedo, n, p, 16.0, 0.05);

	// derivative based terms are computed before branching
	float contourWidth = max(fwidth(height * 1.25), 0.0001);
	float2 gridWidth = max(fwidth(p.xz), float2(0.0001, 0.0001));

	if(height < water){ // water surface
		float3 waterNormal = normalize(float3(
			sin(p.x * 1.7 + getTime() * 1.3) * 0.06,
			1.0,
			cos(p.z * 1.9 + getTime() * 1.1) * 0.06
		));
		float3 deep = lerp(float3(0.02, 0.12, 0.2), float3(0.05, 0.3, 0.35), saturate(1.0 + (height - water) * 0.5));
		float3 reflection = getSkyColor(reflect(-v, waterNormal));
		color = lerp(shadeBlinnPhong(deep, waterNormal, p, 120.0, 1.2), reflection, getFresnel(waterNormal, v, 4.0) * 0.8 + 0.1);
	}

	if(m == 1u){ // topographic lines
		float lines = abs(frac(height * 1.25) - 0.5) / contourWidth;
		color = lerp(color * 0.35, color, saturate(lines));
	} else if(m == 2u){ // wire grid
		float2 g = abs(frac(p.xz - 0.5) - 0.5) / gridWidth;
		float gridLine = 1.0 - saturate(min(g.x, g.y));
		color = lerp(color * 0.25, float3(0.2, 0.9, 1.0), gridLine);
	} else if(m == 3u) color = hsv2rgb(float3(0.66 - saturate((height + 4.0) / 14.0) * 0.66, 0.85, 0.9));

	color = applyFog(color, p);
	return float4(toneMap(color), 1.0);
}
