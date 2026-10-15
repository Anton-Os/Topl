#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

#include "Spacetime.hlsl"

// Spacetime fabric: anti-aliased grid lines tinted by well depth
// mode 0 fabric, mode 1 curvature heat map, mode 2 lines only, mode 3 potential contours
// actor_params.x sets the grid cell size

float4 main(PS_INPUT input) : SV_TARGET {
	uint m = getModes(mode).x % 4u;
	float3 p = input.world_pos;
	float well = input.extra.x;
	float cell = (actor_params.x > 0.0)? actor_params.x : 1.0;

	float2 uv = p.xz / cell;
	float2 g = abs(frac(uv - 0.5) - 0.5) / max(fwidth(uv), float2(0.0001, 0.0001));
	float gridLine = 1.0 - saturate(min(g.x, g.y));

	float contourWidth = max(fwidth(well * 2.0), 0.0001); // computed before branching
	float depth = saturate(well / 6.0);
	float3 lineColor = lerp(float3(0.15, 0.75, 1.0), float3(1.0, 0.3, 0.75), depth);
	float3 base = applyTexture(lerp(float3(0.01, 0.02, 0.06), float3(0.12, 0.02, 0.1), depth), input);

	float3 n = getFacetNormal(p);
	float shade = 0.6 + 0.4 * max(dot(n, getLightDir(p)), 0.0);
	float3 color = base * shade + lineColor * gridLine * (1.2 + depth * 2.0);
	color += lineColor * abs(input.extra.y) * 0.6; // ripples glow

	if(m == 1u) color = hsv2rgb(float3(0.66 - depth * 0.66, 0.9, 0.25 + depth * 0.75)) * shade + (gridLine * 0.25).xxx;
	else if(m == 2u){
		if(gridLine < 0.05) discard;
		color = lineColor * gridLine * 2.0;
	} else if(m == 3u){
		float contour = abs(frac(well * 2.0) - 0.5) / contourWidth;
		color = base + lineColor * (1.0 - saturate(contour)) * 1.5;
	}

	float fade = 1.0 - exp(-length(p - cam_pos.xyz) * fog_color.w);
	color = lerp(color, getSkyColor(p - cam_pos.xyz) * 0.3, saturate(fade));
	return float4(toneMap(color), 1.0);
}
