#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

#include "Tunnel.glsl"

// Torus interior: panels, ribs and pipes lit by a headlamp, drifting strip lights and volumetric rib glow
// mode 0 industrial, mode 1 neon grid, mode 2 organic with breathing walls, mode 3 normals

float getLine(float x, float width){ return 1.0 - smoothstep(0.0, width, abs(x - floor(x + 0.5))); } // 1 on integer values

void main() {
	uint m = getModes(mode).x % 4u;
	bool isWobble = (m == 2u);
	vec3 ro = cam_pos.xyz;
	vec3 rd = normalize(view_ray);
	float far = (getMajor() + getTube()) * 2.5;

	float t = 0.0, glow = 0.0;
	vec3 hit = vec3(-1.0);
	for(int i = 0; i < TUNNEL_STEPS; i++){
		vec3 h = mapTunnel(ro + rd * t, isWobble);
		glow += exp(-max(h.z, 0.0) * 4.0) * 0.02;
		if(h.x < TUNNEL_EPSILON * (1.0 + t)){ hit = vec3(t, h.y, 0.0); break; }
		t += h.x * ((isWobble)? 0.6 : 0.9);
		if(t > far) break;
	}
	if(hit.x < 0.0) hit = vec3(far, 0.0, 0.0);

	vec3 camLocal = toTorus(ro);
	float hueShift = params[5].z;
	vec3 haze = getTunnelLight(getTubeCoords(camLocal).x, hueShift) * 0.08;

	vec3 p = ro + rd * hit.x;
	vec3 n = getTunnelNormal(p, isWobble);
	vec3 l = toTorus(p);
	vec2 uv = getTubeCoords(l);
	float lq = length(vec2(length(l.xz) - getMajor(), l.y));
	float id = hit.y;

	float ribs = getRibCount();
	float alongTorus = uv.x / TWO_PI * ribs * 3.0; // panel coordinates
	float aroundTube = uv.y / TWO_PI * params[4].w;
	float lineWidth = 0.03 + hit.x * 0.0015;
	float seams = max(getLine(alongTorus, lineWidth), getLine(aroundTube, lineWidth));
	float strip = getLine(uv.y / PI - 0.5, 0.025); // light strips on the ceiling and floor
	float ribEdge = (id > 0.5 && id < 1.5)? 1.0 - smoothstep(0.0, 0.07, abs(lq - (getTube() - params[4].x))) : 0.0;
	vec3 lightTone = getTunnelLight(uv.x, hueShift);

	vec3 albedo, emissive;
	if(m == 1u){ // neon
		vec3 hue = hsv2rgb(vec3(fract(uv.x / TWO_PI * 2.0 + getTime() * 0.03 + hueShift), 0.8, 1.0));
		albedo = vec3(0.02, 0.02, 0.03);
		emissive = hue * (seams * 1.4 + ribEdge * 2.5) + hue * strip * 0.6;
		if(id > 1.5) emissive = hue.zxy * 0.8;
	} else if(m == 2u){ // organic
		float hue = fract(uv.x / TWO_PI * 3.0 + uv.y / TWO_PI + getTime() * 0.05 + hueShift);
		albedo = hsv2rgb(vec3(hue, 0.65, 0.85));
		emissive = albedo * ribEdge * 0.8 + hsv2rgb(vec3(hue + 0.5, 0.7, 1.0)) * pow(seams, 3.0) * 0.25;
	} else { // industrial
		float panelTone = 0.35 + 0.12 * hash12(floor(vec2(alongTorus, aroundTube)));
		albedo = vec3(panelTone) * vec3(0.95, 0.97, 1.0) * (1.0 - seams * 0.75);
		if(id > 0.5 && id < 1.5) albedo = vec3(0.18, 0.19, 0.22);
		else if(id > 1.5) albedo = vec3(0.75, 0.42, 0.22);
		emissive = lightTone * (strip * 1.6 * step(id, 0.5) + ribEdge * 1.8);
	}

	vec3 toCam = ro - p; // headlamp at the camera
	float dist = length(toCam);
	vec3 lampDir = toCam / max(dist, 0.0001);
	float lamp = params[5].w / (1.0 + 0.06 * dist * dist);
	float diffuse = max(dot(n, lampDir), 0.0);
	float specular = pow(max(dot(n, lampDir), 0.0), 40.0) * ((id > 1.5)? 1.2 : 0.4);
	vec3 ambient = lightTone * 0.18 + haze;

	vec3 color = albedo * (ambient + vec3(diffuse * lamp)) + vec3(specular * lamp) + emissive;
	color = mix(color, haze * 4.0, 1.0 - exp(-hit.x * 0.04));
	color += lightTone * glow * params[4].z;

	if(m == 3u) color = n * 0.5 + 0.5;
	color_final = vec4((m == 3u)? color : toneMap(color), 1.0);
}
