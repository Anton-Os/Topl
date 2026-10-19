#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

#include "Fractal.hlsl"

// Fractals: sphere traced distance estimators with orbit trap coloring, soft shadows and step based occlusion
// mode 0 shaded, mode 1 neon orbit trap glow, mode 2 normals, mode 3 step count heat map

float4 main(PS_INPUT input) : SV_TARGET {
	uint m = getModes(mode).x % 4u;
	float3 ro = cam_pos.xyz;
	float3 rd = normalize(input.view_ray);
	float pixelAngle = max(length(fwidth(rd)), 1e-5) * 0.7; // cone width of one pixel, keeps detail stable at any distance

	float2 bounds = hitBounds(ro, rd, getBoundRadius());
	float t = max(bounds.x, 0.0);
	float2 hit = float2(-1.0, -1.0);
	float glow = 0.0;
	int steps = 0;
	if(bounds.y > 0.0){
		[loop] for(int i = 0; i < FRACTAL_STEPS; i++){
			float2 h = mapFractal(ro + rd * t);
			steps = i;
			float eps = max(pixelAngle * t, 0.00002 * getWorldScale()) * getDetail();
			glow += exp(-h.x * 6.0 / getWorldScale()) * 0.012;
			if(h.x < eps){ hit = float2(t, h.y); break; }
			t += h.x * 0.9;
			if(t > bounds.y) break;
		}
	}

	float3 background = getSkyColor(rd) * 0.35;
	float3 color = background;
	float stepRatio = float(steps) / float(FRACTAL_STEPS);
	float3 glowColor = getFractalPalette(0.6 + getTime() * 0.02) * glow * params[1].y;

	if(hit.x >= 0.0){
		float3 p = ro + rd * hit.x;
		float eps = max(pixelAngle * hit.x, 0.00002 * getWorldScale()) * getDetail();
		float3 n = getFractalNormal(p, eps * 0.5);
		float3 l = getLightDir(p);
		float3 albedo = getFractalPalette(saturate(sqrt(hit.y) * 0.45));

		float occ = saturate(1.0 - stepRatio * 1.6);
		float shadow = lerp(1.0, getFractalShadow(p + n * eps * 4.0, l, eps * 8.0), saturate(params[2].z));
		float diffuse = max(dot(n, l), 0.0) * shadow;
		float specular = pow(max(dot(n, normalize(l - rd)), 0.0), 48.0) * shadow;
		float rim = getFresnel(n, -rd, 3.0);

		color = albedo * (getAmbient(n) * (0.4 + occ) + light_color.rgb * diffuse * getLightAtten(p)) + light_color.rgb * specular * 0.4;
		color += albedo * rim * occ * 0.25;
		if(m == 1u) color = albedo * (0.15 + occ * 0.2) + getFractalPalette(hit.y * 0.3 + getTime() * 0.05) * pow(1.0 - occ, 2.0) * 1.5;
		else if(m == 2u) color = n * 0.5 + 0.5;
	} else if(m == 1u) color = background * 0.3;

	if(m == 3u) color = hsv2rgb(float3(0.66 - stepRatio * 0.66, 0.9, 0.9));
	else if(m != 2u) color += glowColor * ((m == 1u)? 2.0 : 1.0);

	return float4((m == 2u || m == 3u)? color : toneMap(color), 1.0);
}
