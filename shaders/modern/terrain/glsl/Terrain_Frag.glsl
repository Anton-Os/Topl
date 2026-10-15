#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

// Terrain: height and slope based materials, reflective water, distance fog
// mode 0 natural, mode 1 topographic contours, mode 2 wire grid, mode 3 height map

void main() {
	uint m = getModes(mode).x % 4u;
	float height = extra.x;
	float water = extra.y;
	vec3 n = getFacetNormal();
	vec3 v = normalize(cam_pos.xyz - world_pos);
	float slope = 1.0 - n.y;
	float rel = height - water;

	vec3 grass = mix(vec3(0.18, 0.36, 0.12), vec3(0.32, 0.45, 0.16), valueNoise(world_pos.xz * 0.35));
	vec3 rock = mix(vec3(0.33, 0.3, 0.27), vec3(0.45, 0.42, 0.38), valueNoise(world_pos.xz * 1.3));
	vec3 sand = vec3(0.76, 0.68, 0.5);
	vec3 snow = vec3(0.92, 0.94, 0.98);

	vec3 albedo = mix(sand, grass, smoothstep(0.1, 0.6, rel));
	albedo = mix(albedo, rock, smoothstep(0.25, 0.5, slope));
	albedo = mix(albedo, snow, smoothstep(4.5, 6.0, rel + valueNoise(world_pos.xz * 0.5) * 1.5) * (1.0 - smoothstep(0.45, 0.7, slope)));
	albedo = applyTexture(albedo);

	vec3 color = shadeBlinnPhong(albedo, n, world_pos, 16.0, 0.05);

	// derivative based terms are computed before branching
	float contourWidth = max(fwidth(height * 1.25), 0.0001);
	vec2 gridWidth = max(fwidth(world_pos.xz), vec2(0.0001));

	if(height < water){ // water surface
		vec3 waterNormal = normalize(vec3(
			sin(world_pos.x * 1.7 + getTime() * 1.3) * 0.06,
			1.0,
			cos(world_pos.z * 1.9 + getTime() * 1.1) * 0.06
		));
		vec3 deep = mix(vec3(0.02, 0.12, 0.2), vec3(0.05, 0.3, 0.35), clamp(1.0 + (height - water) * 0.5, 0.0, 1.0));
		vec3 reflection = getSkyColor(reflect(-v, waterNormal));
		color = mix(shadeBlinnPhong(deep, waterNormal, world_pos, 120.0, 1.2), reflection, getFresnel(waterNormal, v, 4.0) * 0.8 + 0.1);
	}

	if(m == 1u){ // topographic lines
		float lines = abs(fract(height * 1.25) - 0.5) / contourWidth;
		color = mix(color * 0.35, color, clamp(lines, 0.0, 1.0));
	} else if(m == 2u){ // wire grid
		vec2 g = abs(fract(world_pos.xz - 0.5) - 0.5) / gridWidth;
		float gridLine = 1.0 - clamp(min(g.x, g.y), 0.0, 1.0);
		color = mix(color * 0.25, vec3(0.2, 0.9, 1.0), gridLine);
	} else if(m == 3u) color = hsv2rgb(vec3(0.66 - clamp((height + 4.0) / 14.0, 0.0, 1.0) * 0.66, 0.85, 0.9));

	color = applyFog(color, world_pos);
	color_final = vec4(toneMap(color), 1.0);
}
