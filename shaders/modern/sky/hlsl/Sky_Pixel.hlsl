#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

#include "Sky.hlsl"

// Main

float4 main(PS_INPUT input) : SV_TARGET {
	float3 dir = normalize(input.view_ray);
	float3 color;

	uint m = getModes(mode).x % 4u;
	if(m == 1u) color = getCloudySky(dir);
	else if(m == 2u) color = getNightSky(dir);
	else if(m == 3u) color = getNightSky(dir) + getAurora(dir);
	else color = getSkyColor(dir);

	return float4(toneMap(color), 1.0);
}
