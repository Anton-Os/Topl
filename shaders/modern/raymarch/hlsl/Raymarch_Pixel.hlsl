#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

#include "Raymarch.hlsl"

// Raymarching: sphere traced signed distance scene with soft shadows and ambient occlusion
// mode 0 shaded, mode 1 normals, mode 2 step count heat map, mode 3 occlusion

float4 main(PS_INPUT input) : SV_TARGET {
	uint m = getModes(mode).x % 4u;
	float3 ro = cam_pos.xyz;
	float3 rd = normalize(input.view_ray);

	float t = 0.0;
	float2 hit = float2(MARCH_FAR, -1.0);
	int steps = 0;
	[loop] for(int i = 0; i < MARCH_STEPS; i++){
		float2 h = mapScene(ro + rd * t);
		steps = i;
		if(h.x < MARCH_EPSILON * t){ hit = float2(t, h.y); break; }
		t += h.x;
		if(t > MARCH_FAR) break;
	}

	float3 color;
	if(hit.y < 0.0) color = getSkyColor(rd);
	else {
		float3 p = ro + rd * hit.x;
		float3 n = getSceneNormal(p);
		float3 l = getLightDir(p);
		float3 albedo = getMaterial(hit.y, p);

		float shadow = getSoftShadow(p + n * 0.01, l);
		float occ = getOcclusion(p, n);
		float diffuse = max(dot(n, l), 0.0) * shadow;
		float specular = pow(max(dot(n, normalize(l - rd)), 0.0), 64.0) * shadow;

		color = albedo * (getAmbient(n) * occ + light_color.rgb * diffuse * getLightAtten(p)) + light_color.rgb * specular * 0.5;
		color = lerp(color, getSkyColor(reflect(rd, n)), getFresnel(n, -rd, 5.0) * 0.25 * occ);

		float fogAmount = 1.0 - exp(-hit.x * fog_color.w);
		color = lerp(color, getSkyColor(rd), saturate(fogAmount));

		if(m == 1u) color = n * 0.5 + 0.5;
		else if(m == 3u) color = (occ * (0.35 + 0.65 * shadow)).xxx;
	}
	if(m == 2u) color = hsv2rgb(float3(0.66 - float(steps) / float(MARCH_STEPS) * 0.66, 0.9, 0.9));

	return float4((m == 0u)? toneMap(color) : color, 1.0);
}
