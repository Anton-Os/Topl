#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

#include "Spacetime.glsl"

// Spacetime fabric: anti-aliased grid lines tinted by well depth
// mode 0 fabric, mode 1 curvature heat map, mode 2 lines only, mode 3 potential contours
// actor_params.x sets the grid cell size

void main() {
	uint m = getModes(mode).x % 4u;
	float well = extra.x;
	float cell = (actor_params.x > 0.0)? actor_params.x : 1.0;

	vec2 uv = world_pos.xz / cell;
	vec2 g = abs(fract(uv - 0.5) - 0.5) / max(fwidth(uv), vec2(0.0001));
	float gridLine = 1.0 - clamp(min(g.x, g.y), 0.0, 1.0);

	float contourWidth = max(fwidth(well * 2.0), 0.0001); // computed before branching
	float depth = clamp(well / 6.0, 0.0, 1.0);
	vec3 lineColor = mix(vec3(0.15, 0.75, 1.0), vec3(1.0, 0.3, 0.75), depth);
	vec3 base = applyTexture(mix(vec3(0.01, 0.02, 0.06), vec3(0.12, 0.02, 0.1), depth));

	vec3 n = getFacetNormal();
	float shade = 0.6 + 0.4 * max(dot(n, getLightDir(world_pos)), 0.0);
	vec3 color = base * shade + lineColor * gridLine * (1.2 + depth * 2.0);
	color += lineColor * abs(extra.y) * 0.6; // ripples glow

	if(m == 1u) color = hsv2rgb(vec3(0.66 - depth * 0.66, 0.9, 0.25 + depth * 0.75)) * shade + vec3(gridLine * 0.25);
	else if(m == 2u){
		if(gridLine < 0.05) discard;
		color = lineColor * gridLine * 2.0;
	} else if(m == 3u){
		float contour = abs(fract(well * 2.0) - 0.5) / contourWidth;
		color = base + lineColor * (1.0 - clamp(contour, 0.0, 1.0)) * 1.5;
	}

	float fade = 1.0 - exp(-length(world_pos - cam_pos.xyz) * fog_color.w);
	color = mix(color, getSkyColor(world_pos - cam_pos.xyz) * 0.3, clamp(fade, 0.0, 1.0));
	color_final = vec4(toneMap(color), 1.0);
}
