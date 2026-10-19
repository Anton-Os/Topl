#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

#include "Planet.hlsl"

// Planet shading: biomes picked from height, slope and latitude, animated oceans and an atmospheric rim
// mode 0 natural, mode 1 topographic contours, mode 2 normals, mode 3 height heat map

float3 getLandColor(int biome, float hs, float slope, float lat, out float3 emissive){
	emissive = float3(0.0, 0.0, 0.0);
	float3 rock = float3(0.32, 0.3, 0.28);
	float3 color;
	if(biome == 1){ // desert
		color = lerp(float3(0.82, 0.55, 0.32), float3(0.62, 0.32, 0.18), smoothstep(0.1, 0.5, hs));
		color = lerp(color, float3(0.9, 0.82, 0.7), smoothstep(0.75, 0.95, hs));
		rock = float3(0.4, 0.22, 0.15);
	} else if(biome == 2){ // ice
		color = lerp(float3(0.75, 0.85, 0.95), float3(0.95, 0.97, 1.0), smoothstep(0.0, 0.5, hs));
		rock = float3(0.35, 0.45, 0.6);
	} else if(biome == 3){ // volcanic
		color = lerp(float3(0.08, 0.07, 0.07), float3(0.22, 0.2, 0.2), smoothstep(0.1, 0.7, hs));
		rock = float3(0.12, 0.1, 0.1);
		float vents = smoothstep(0.82, 0.95, hs) + smoothstep(0.2, 0.0, hs) * 0.6;
		emissive = float3(1.0, 0.35, 0.05) * vents * (0.6 + 0.4 * sin(getTime() * 3.0 + hs * 40.0));
	} else { // earth
		color = lerp(float3(0.76, 0.7, 0.5), float3(0.3, 0.55, 0.22), smoothstep(0.02, 0.08, hs)); // beach to grass
		color = lerp(color, float3(0.16, 0.35, 0.14), smoothstep(0.15, 0.35, hs)); // forest
		color = lerp(color, float3(0.45, 0.42, 0.38), smoothstep(0.5, 0.65, hs)); // highland
		color = lerp(color, float3(0.95, 0.96, 1.0), smoothstep(0.78, 0.86, hs + lat * 0.35)); // snow caps and poles
	}
	return lerp(color, rock, smoothstep(0.25, 0.45, slope) * (1.0 - smoothstep(0.85, 0.95, hs + lat * 0.3) * ((biome == 0)? 1.0 : 0.0)));
}

float3 getWaterColor(int biome, float depth, out float3 emissive){
	emissive = float3(0.0, 0.0, 0.0);
	if(biome == 1) return lerp(float3(0.7, 0.45, 0.28), float3(0.55, 0.3, 0.2), smoothstep(0.0, 3.0, depth)); // dust basins
	if(biome == 2) return lerp(float3(0.55, 0.75, 0.85), float3(0.2, 0.35, 0.55), smoothstep(0.0, 4.0, depth)); // frozen seas
	if(biome == 3){ // lava seas
		emissive = lerp(float3(1.0, 0.55, 0.1), float3(0.8, 0.12, 0.02), smoothstep(0.0, 3.0, depth)) * (1.4 + 0.4 * sin(getTime() * 1.7 + depth));
		return float3(0.1, 0.03, 0.02);
	}
	return lerp(float3(0.1, 0.45, 0.55), float3(0.02, 0.1, 0.3), smoothstep(0.0, 5.0, depth));
}

float4 main(PS_INPUT input) : SV_TARGET {
	uint m = getModes(mode).x % 4u;
	float3 dir = normalize(input.texcoord);
	float h = input.extra.x;
	float sea = input.extra.y;
	int biome = int(params[4].w + 0.5) % 4;
	float contourWidth = max(fwidth(h), 0.0001); // derivatives are taken before branching
	float3 facet = getFacetNormal(input.world_pos);

	float3 up = planetToWorld(dir);
	float3 n = (dot(input.world_normal, input.world_normal) > 0.0001)? normalize(input.world_normal) : facet;
	float slope = 1.0 - clamp(dot(n, up), 0.0, 1.0);
	float hs = (h - sea) / (getHeightScale() * 2.2); // roughly 0 at the shore and 1 on the highest peaks
	float lat = abs(dir.y);
	bool isWater = h <= sea + 0.0001;
	float3 v = normalize(cam_pos.xyz - input.world_pos);

	float3 albedo, emissive;
	float shininess = 16.0, specStrength = 0.1;
	if(isWater){
		albedo = getWaterColor(biome, sea - h, emissive);
		if(biome == 0 || biome == 2){ // rolling waves
			float3 t1 = planetToWorld(planetTangent(dir));
			float3 t2 = cross(up, t1);
			float w1 = valueNoise3(dir * 90.0 + float3(getTime() * 0.6, 0.0, 0.0));
			float w2 = valueNoise3(dir * 90.0 + float3(5.2, getTime() * 0.5, 1.7));
			n = normalize(up + (t1 * (w1 - 0.5) + t2 * (w2 - 0.5)) * 0.35 * params[5].w);
			shininess = 96.0; specStrength = 0.9;
		}
	} else albedo = getLandColor(biome, hs, slope, lat, emissive);

	float3 color = shadeBlinnPhong(albedo, n, input.world_pos, shininess, specStrength) + emissive;
	if(isWater && biome == 0) color = lerp(color, getSkyColor(reflect(-v, n)), getFresnel(n, v, 5.0) * 0.6);
	color += getSkyColor(-v) * getFresnel(up, v, 4.0) * 0.25; // atmosphere rim seen from orbit

	if(m == 1u){ // contour lines every world unit, thicker every five
		float minor = 1.0 - smoothstep(0.0, contourWidth * 1.5, abs(frac(h + 0.5) - 0.5));
		float major = 1.0 - smoothstep(0.0, contourWidth * 2.5, abs(frac(h / 5.0 + 0.5) - 0.5) * 5.0);
		color = lerp(color, float3(0.05, 0.05, 0.05), saturate(minor * 0.5 + major) * ((isWater)? 0.0 : 1.0));
	} else if(m == 2u) color = n * 0.5 + 0.5;
	else if(m == 3u) color = hsv2rgb(float3(0.66 - clamp(hs, 0.0, 1.0) * 0.66, 0.85, (isWater)? 0.35 : 0.9));

	if(m < 2u) color = applyFog(color, input.world_pos);
	return float4((m == 2u)? color : toneMap(color), 1.0);
}
