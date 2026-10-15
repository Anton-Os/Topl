#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

#include "Spectrum.glsl"

// Audio reactive colors: band hue, level driven glow, neon floor rings
// mode 0 neon, mode 1 lit, mode 2 monochrome pulse, mode 3 level heat map

void main() {
	uint m = getModes(mode).x % 4u;
	float band = extra.x;
	float level = extra.y;
	int kind = int(extra.z + 0.5);

	vec3 n = getSurfaceNormal(kind != 2);
	vec3 v = normalize(cam_pos.xyz - world_pos);
	vec3 hue = applyTexture(getBandColor(band));
	vec3 color;

	float dist = length(world_pos.xz); // derivative based terms are computed before branching on the varying kind
	float ringWidth = max(fwidth(dist * 0.5), 0.0001);
	vec2 gridWidth = max(fwidth(world_pos.xz), vec2(0.0001));

	if(kind == 1){ // floor
		float ring = abs(fract(dist * 0.5 - getTime() * 0.5) - 0.5) / ringWidth;
		vec2 g = abs(fract(world_pos.xz - 0.5) - 0.5) / gridWidth;
		float grid = 1.0 - clamp(min(g.x, g.y), 0.0, 1.0);
		color = vec3(0.01, 0.01, 0.03) + hue * ((1.0 - clamp(ring, 0.0, 1.0)) * (0.3 + getBass() * 1.5) + grid * 0.15) * (0.3 + level);
	} else {
		float rim = getFresnel(n, v, 2.0);
		vec3 lit = shadeBlinnPhong(hue * 0.6, n, world_pos, 64.0, 0.8);
		if(m == 1u) color = lit + hue * level * 0.3;
		else color = hue * (0.15 + level * 1.6) + hue * rim * (0.6 + getLevel()) + lit * 0.25;
	}

	if(m == 2u) color = vec3(dot(color, vec3(0.299, 0.587, 0.114))) * vec3(0.8, 0.9, 1.0) * (0.6 + getLevel());
	else if(m == 3u) color = hsv2rgb(vec3(0.66 - clamp(level, 0.0, 1.0) * 0.66, 0.9, 0.3 + level));

	float fade = 1.0 - exp(-length(world_pos - cam_pos.xyz) * fog_color.w);
	color = mix(color, fog_color.rgb * 0.1, clamp(fade, 0.0, 1.0));
	color_final = vec4(toneMap(color), 1.0);
}
