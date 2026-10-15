#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

#include "Spectrum.hlsl"

// Audio reactive colors: band hue, level driven glow, neon floor rings
// mode 0 neon, mode 1 lit, mode 2 monochrome pulse, mode 3 level heat map

float4 main(PS_INPUT input) : SV_TARGET {
	uint m = getModes(mode).x % 4u;
	float3 p = input.world_pos;
	float band = input.extra.x;
	float level = input.extra.y;
	int kind = int(input.extra.z + 0.5);

	float3 n = getSurfaceNormal(input, kind != 2);
	float3 v = normalize(cam_pos.xyz - p);
	float3 hue = applyTexture(getBandColor(band), input);
	float3 color;

	float dist = length(p.xz); // derivative based terms are computed before branching on the varying kind
	float ringWidth = max(fwidth(dist * 0.5), 0.0001);
	float2 gridWidth = max(fwidth(p.xz), float2(0.0001, 0.0001));

	if(kind == 1){ // floor
		float ring = abs(frac(dist * 0.5 - getTime() * 0.5) - 0.5) / ringWidth;
		float2 g = abs(frac(p.xz - 0.5) - 0.5) / gridWidth;
		float grid = 1.0 - saturate(min(g.x, g.y));
		color = float3(0.01, 0.01, 0.03) + hue * ((1.0 - saturate(ring)) * (0.3 + getBass() * 1.5) + grid * 0.15) * (0.3 + level);
	} else {
		float rim = getFresnel(n, v, 2.0);
		float3 lit = shadeBlinnPhong(hue * 0.6, n, p, 64.0, 0.8);
		if(m == 1u) color = lit + hue * level * 0.3;
		else color = hue * (0.15 + level * 1.6) + hue * rim * (0.6 + getLevel()) + lit * 0.25;
	}

	if(m == 2u) color = dot(color, float3(0.299, 0.587, 0.114)).xxx * float3(0.8, 0.9, 1.0) * (0.6 + getLevel());
	else if(m == 3u) color = hsv2rgb(float3(0.66 - saturate(level) * 0.66, 0.9, 0.3 + level));

	float fade = 1.0 - exp(-length(p - cam_pos.xyz) * fog_color.w);
	color = lerp(color, fog_color.rgb * 0.1, saturate(fade));
	return float4(toneMap(color), 1.0);
}
