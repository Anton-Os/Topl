#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

#include "Sky.glsl"

// Main

void main() {
	vec3 dir = normalize(view_ray);
	vec3 color;

	uint m = getModes(mode).x % 4u;
	if(m == 1u) color = getCloudySky(dir);
	else if(m == 2u) color = getNightSky(dir);
	else if(m == 3u) color = getNightSky(dir) + getAurora(dir);
	else color = getSkyColor(dir);

	color_final = vec4(toneMap(color), 1.0);
}
