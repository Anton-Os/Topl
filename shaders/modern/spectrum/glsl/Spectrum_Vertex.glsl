#version 440

#define STAGE_VERTEX

#include "Modern_Common.glsl"

#include "Modern_Vertex.glsl"

#include "Spectrum.glsl"

// Audio reactive displacement
// actor_params: x band index, y kind (0 bar, 1 rippling floor, 2 pulsing orb)

void main() {
	int kind = int(actor_params.y + 0.5);
	vec3 local = pos_in;
	float band = actor_params.x;
	float level = getBandSmooth(band);

	if(kind == 2){ // orb: each direction is pushed out by the band matching its angle
		vec3 dir = (dot(local, local) > 0.0)? normalize(local) : vec3(0.0, 1.0, 0.0);
		float angle = fract(atan(dir.z, dir.x) / TWO_PI + 0.5);
		float mirrored = 1.0 - abs(angle * 2.0 - 1.0); // seamless wrap around the orb
		band = mirrored * float(SPECTRUM_BANDS - 1);
		level = getBandSmooth(band);
		float latitude = 1.0 - abs(dir.y);
		local += dir * (level * 0.55 * latitude + getTreble() * 0.15 * (1.0 - latitude) + getBass() * 0.2) * length(pos_in);
	}

	vec3 worldPos = getWorldPos(local);

	if(kind == 1){ // floor: rings expanding from the center driven by bass
		float dist = length(worldPos.xz);
		worldPos.y += sin(dist * 1.4 - getTime() * 4.0) * getBass() * 0.6 / (1.0 + dist * 0.15);
		band = clamp(dist / 2.5, 0.0, float(SPECTRUM_BANDS - 1));
		level = getBandSmooth(band);
	}

	gl_Position = fpsTransform(worldPos);
	passVaryings(worldPos, (kind == 2)? getWorldNormal(local) : vec3(0.0)); // radial normals for the orb, facets elsewhere
	extra = vec4(band, level, float(kind), 0.0);
}
