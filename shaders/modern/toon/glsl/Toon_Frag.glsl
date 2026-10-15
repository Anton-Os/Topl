#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

// Toon shading: banded diffuse, hard specular, rim light and ink silhouette
// mode 0 cel, mode 1 cel with halftone shadows, mode 2 cel with hatching, mode 3 flat bands only

float getBands(float value, float count){ return floor(value * count + 0.5) / count; }

void main() {
	uint m = getModes(mode).x % 4u;
	vec3 n = getSurfaceNormal(false);
	vec3 l = getLightDir(world_pos);
	vec3 v = normalize(cam_pos.xyz - world_pos);
	vec3 h = normalize(l + v);
	vec4 albedo = getActorColor();
	albedo.rgb = applyTexture(albedo.rgb);

	float diffuse = max(dot(n, l), 0.0) * getLightAtten(world_pos);
	float banded = getBands(clamp(diffuse, 0.0, 1.0), 3.0);
	float specular = step(0.96, max(dot(n, h), 0.0)) * step(0.01, diffuse);
	float rim = smoothstep(0.62, 0.7, getFresnel(n, v, 1.0)) * clamp(diffuse + 0.3, 0.0, 1.0);

	vec3 shadowTone = albedo.rgb * mix(vec3(0.25, 0.22, 0.4), fog_color.rgb, 0.3);
	vec3 color = mix(shadowTone, albedo.rgb * light_color.rgb, banded);

	vec2 screen = gl_FragCoord.xy;
	if(m == 1u){ // halftone dots in shadow
		vec2 cell = fract(screen / 7.0) - 0.5;
		float dotSize = (1.0 - banded) * 0.55;
		color *= (length(cell) < dotSize)? 0.55 : 1.0;
	} else if(m == 2u){ // diagonal hatching in shadow
		float hatch = step(0.5, fract((screen.x + screen.y) / 6.0));
		if(banded < 0.34) color *= mix(1.0, 0.5, hatch);
		if(banded < 0.01) color *= mix(1.0, 0.6, step(0.5, fract((screen.x - screen.y) / 6.0)));
	}

	if(m != 3u){
		color += vec3(specular) * 0.8 + light_color.rgb * rim * 0.35;
		float edge = clamp(dot(n, v), 0.0, 1.0);
		if(edge < 0.22) color = mix(vec3(0.02), color, edge / 0.22 * 0.35); // ink outline
	}

	color = applyFog(color, world_pos);
	color_final = vec4(toneMap(color * 1.15), albedo.a);
}
