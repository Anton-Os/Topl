#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

// Main
// actor_params: x shininess, y reflectivity, z emissive strength, w smooth sphere normals

void main() {
	uint m = getModes(mode).x % 4u;
	vec3 n = getSurfaceNormal(m == 1u);
	vec3 v = normalize(cam_pos.xyz - world_pos);
	vec4 albedo = getActorColor();
	albedo.rgb = applyTexture(albedo.rgb);

	float shininess = (actor_params.x > 0.0)? actor_params.x : 48.0;
	float reflectivity = clamp(actor_params.y, 0.0, 1.0);
	float emissive = max(actor_params.z, 0.0);

	vec3 color = shadeBlinnPhong(albedo.rgb, n, world_pos, shininess, 0.6);
	vec3 reflection = getSkyColor(reflect(-v, n));
	color = mix(color, reflection, getFresnel(n, v, 5.0) * (0.15 + reflectivity * 0.85));
	color += albedo.rgb * emissive;

	if(m == 2u) color = n * 0.5 + 0.5; // normal view
	else if(m == 3u) color = vec3(1.0 - clamp(extra.x / (max(cam_lens.w, 1.0) * 0.25), 0.0, 1.0)); // depth view
	else color = applyFog(color, world_pos);

	color_final = vec4((m >= 2u)? color : toneMap(color), albedo.a);
}
