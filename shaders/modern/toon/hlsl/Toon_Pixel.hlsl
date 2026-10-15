#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

// Toon shading: banded diffuse, hard specular, rim light and ink silhouette
// mode 0 cel, mode 1 cel with halftone shadows, mode 2 cel with hatching, mode 3 flat bands only

float getBands(float value, float count){ return floor(value * count + 0.5) / count; }

float4 main(PS_INPUT input) : SV_TARGET {
	uint m = getModes(mode).x % 4u;
	float3 n = getSurfaceNormal(input, false);
	float3 l = getLightDir(input.world_pos);
	float3 v = normalize(cam_pos.xyz - input.world_pos);
	float3 h = normalize(l + v);
	float4 albedo = getActorColor();
	albedo.rgb = applyTexture(albedo.rgb, input);

	float diffuse = max(dot(n, l), 0.0) * getLightAtten(input.world_pos);
	float banded = getBands(saturate(diffuse), 3.0);
	float specular = step(0.96, max(dot(n, h), 0.0)) * step(0.01, diffuse);
	float rim = smoothstep(0.62, 0.7, getFresnel(n, v, 1.0)) * saturate(diffuse + 0.3);

	float3 shadowTone = albedo.rgb * lerp(float3(0.25, 0.22, 0.4), fog_color.rgb, 0.3);
	float3 color = lerp(shadowTone, albedo.rgb * light_color.rgb, banded);

	float2 screen = input.pos.xy;
	if(m == 1u){ // halftone dots in shadow
		float2 cell = frac(screen / 7.0) - 0.5;
		float dotSize = (1.0 - banded) * 0.55;
		color *= (length(cell) < dotSize)? 0.55 : 1.0;
	} else if(m == 2u){ // diagonal hatching in shadow
		float hatch = step(0.5, frac((screen.x + screen.y) / 6.0));
		if(banded < 0.34) color *= lerp(1.0, 0.5, hatch);
		if(banded < 0.01) color *= lerp(1.0, 0.6, step(0.5, frac((screen.x - screen.y) / 6.0)));
	}

	if(m != 3u){
		color += specular.xxx * 0.8 + light_color.rgb * rim * 0.35;
		float edge = saturate(dot(n, v));
		if(edge < 0.22) color = lerp(float3(0.02, 0.02, 0.02), color, edge / 0.22 * 0.35); // ink outline
	}

	color = applyFog(color, input.world_pos);
	return float4(toneMap(color * 1.15), albedo.a);
}
