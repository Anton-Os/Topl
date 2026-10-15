#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

// Hologram: fresnel rim, moving scanlines, flicker and glitch tint
// mode 0 scanlines, mode 1 data rain, mode 2 stable glow, mode 3 interference bands

void main() {
	uint m = getModes(mode).x % 4u;
	vec3 n = getSurfaceNormal(false);
	vec3 v = normalize(cam_pos.xyz - world_pos);
	vec4 tint = getActorColor();
	tint.rgb = applyTexture(tint.rgb);
	float t = getTime();

	float rim = getFresnel(n, v, 2.5);
	float scan = 0.5 + 0.5 * sin(world_pos.y * 90.0 - t * 8.0);
	float sweep = smoothstep(0.0, 0.06, abs(fract(world_pos.y * 0.25 - t * 0.35) - 0.5)); // slow scanning band
	float flicker = 0.85 + 0.15 * hash12(vec2(floor(t * 24.0), 3.0));

	float intensity = 0.12 + rim * 1.4;
	if(m == 0u) intensity += scan * 0.35 + (1.0 - sweep) * 0.8;
	else if(m == 1u){
		vec2 cell = floor(vec2(world_pos.x + world_pos.z, world_pos.y) * vec2(14.0, 18.0));
		float speed = 2.0 + hash12(vec2(cell.x, 1.0)) * 4.0;
		float drop = fract(hash12(vec2(cell.x, 9.0)) - (cell.y / 18.0) * 0.4 + t * speed * 0.1);
		intensity += smoothstep(0.85, 1.0, drop) * 1.2;
	} else if(m == 3u) intensity += 0.5 + 0.5 * sin((world_pos.x + world_pos.y + world_pos.z) * 12.0 + t * 3.0);
	else intensity += 0.4;

	vec3 color = tint.rgb * intensity * flicker;
	if(abs(extra.x) > 0.0001) color = mix(color, vec3(1.0, 0.25, 0.6), 0.6); // glitched slices flash magenta

	float alpha = clamp(0.25 + rim * 0.7 + scan * 0.1, 0.0, 1.0) * tint.a;
	color_final = vec4(toneMap(color), alpha);
}
