#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

#include "Raymarch.glsl"

// Raymarching: sphere traced signed distance scene with soft shadows and ambient occlusion
// mode 0 shaded, mode 1 normals, mode 2 step count heat map, mode 3 occlusion

void main() {
	uint m = getModes(mode).x % 4u;
	vec3 ro = cam_pos.xyz;
	vec3 rd = normalize(view_ray);

	float t = 0.0;
	vec2 hit = vec2(MARCH_FAR, -1.0);
	int steps = 0;
	for(int i = 0; i < MARCH_STEPS; i++){
		vec2 h = mapScene(ro + rd * t);
		steps = i;
		if(h.x < MARCH_EPSILON * t){ hit = vec2(t, h.y); break; }
		t += h.x;
		if(t > MARCH_FAR) break;
	}

	vec3 color;
	if(hit.y < 0.0) color = getSkyColor(rd);
	else {
		vec3 p = ro + rd * hit.x;
		vec3 n = getSceneNormal(p);
		vec3 l = getLightDir(p);
		vec3 albedo = getMaterial(hit.y, p);

		float shadow = getSoftShadow(p + n * 0.01, l);
		float occ = getOcclusion(p, n);
		float diffuse = max(dot(n, l), 0.0) * shadow;
		float specular = pow(max(dot(n, normalize(l - rd)), 0.0), 64.0) * shadow;

		color = albedo * (getAmbient(n) * occ + light_color.rgb * diffuse * getLightAtten(p)) + light_color.rgb * specular * 0.5;
		color = mix(color, getSkyColor(reflect(rd, n)), getFresnel(n, -rd, 5.0) * 0.25 * occ);

		float fogAmount = 1.0 - exp(-hit.x * fog_color.w);
		color = mix(color, getSkyColor(rd), clamp(fogAmount, 0.0, 1.0));

		if(m == 1u) color = n * 0.5 + 0.5;
		else if(m == 3u) color = vec3(occ * (0.35 + 0.65 * shadow));
	}
	if(m == 2u) color = hsv2rgb(vec3(0.66 - float(steps) / float(MARCH_STEPS) * 0.66, 0.9, 0.9));

	color_final = vec4((m == 0u)? toneMap(color) : color, 1.0);
}
