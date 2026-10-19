#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

#include "Tunnel.hlsl"

// Torus interior: panels, ribs and pipes lit by a headlamp, drifting strip lights and volumetric rib glow
// mode 0 industrial, mode 1 neon grid, mode 2 organic with breathing walls, mode 3 normals

float getLine(float x, float width){ return 1.0 - smoothstep(0.0, width, abs(x - floor(x + 0.5))); } // 1 on integer values

float4 main(PS_INPUT input) : SV_TARGET {
	uint m = getModes(mode).x % 4u;
	bool isWobble = (m == 2u);
	float3 ro = cam_pos.xyz;
	float3 rd = normalize(input.view_ray);
	float far = (getMajor() + getTube()) * 2.5;

	float t = 0.0, glow = 0.0;
	float3 hit = float3(-1.0, -1.0, -1.0);
	[loop] for(int i = 0; i < TUNNEL_STEPS; i++){
		float3 h = mapTunnel(ro + rd * t, isWobble);
		glow += exp(-max(h.z, 0.0) * 4.0) * 0.02;
		if(h.x < TUNNEL_EPSILON * (1.0 + t)){ hit = float3(t, h.y, 0.0); break; }
		t += h.x * ((isWobble)? 0.6 : 0.9);
		if(t > far) break;
	}
	if(hit.x < 0.0) hit = float3(far, 0.0, 0.0);

	float3 camLocal = toTorus(ro);
	float hueShift = params[5].z;
	float3 haze = getTunnelLight(getTubeCoords(camLocal).x, hueShift) * 0.08;

	float3 p = ro + rd * hit.x;
	float3 n = getTunnelNormal(p, isWobble);
	float3 l = toTorus(p);
	float2 uv = getTubeCoords(l);
	float lq = length(float2(length(l.xz) - getMajor(), l.y));
	float id = hit.y;

	float ribs = getRibCount();
	float alongTorus = uv.x / TWO_PI * ribs * 3.0; // panel coordinates
	float aroundTube = uv.y / TWO_PI * params[4].w;
	float lineWidth = 0.03 + hit.x * 0.0015;
	float seams = max(getLine(alongTorus, lineWidth), getLine(aroundTube, lineWidth));
	float strip = getLine(uv.y / PI - 0.5, 0.025); // light strips on the ceiling and floor
	float ribEdge = (id > 0.5 && id < 1.5)? 1.0 - smoothstep(0.0, 0.07, abs(lq - (getTube() - params[4].x))) : 0.0;
	float3 lightTone = getTunnelLight(uv.x, hueShift);

	float3 albedo, emissive;
	if(m == 1u){ // neon
		float3 hue = hsv2rgb(float3(frac(uv.x / TWO_PI * 2.0 + getTime() * 0.03 + hueShift), 0.8, 1.0));
		albedo = float3(0.02, 0.02, 0.03);
		emissive = hue * (seams * 1.4 + ribEdge * 2.5) + hue * strip * 0.6;
		if(id > 1.5) emissive = hue.zxy * 0.8;
	} else if(m == 2u){ // organic
		float hue = frac(uv.x / TWO_PI * 3.0 + uv.y / TWO_PI + getTime() * 0.05 + hueShift);
		albedo = hsv2rgb(float3(hue, 0.65, 0.85));
		emissive = albedo * ribEdge * 0.8 + hsv2rgb(float3(hue + 0.5, 0.7, 1.0)) * pow(seams, 3.0) * 0.25;
	} else { // industrial
		float panelTone = 0.35 + 0.12 * hash12(floor(float2(alongTorus, aroundTube)));
		albedo = panelTone.xxx * float3(0.95, 0.97, 1.0) * (1.0 - seams * 0.75);
		if(id > 0.5 && id < 1.5) albedo = float3(0.18, 0.19, 0.22);
		else if(id > 1.5) albedo = float3(0.75, 0.42, 0.22);
		emissive = lightTone * (strip * 1.6 * step(id, 0.5) + ribEdge * 1.8);
	}

	float3 toCam = ro - p; // headlamp at the camera
	float dist = length(toCam);
	float3 lampDir = toCam / max(dist, 0.0001);
	float lamp = params[5].w / (1.0 + 0.06 * dist * dist);
	float diffuse = max(dot(n, lampDir), 0.0);
	float specular = pow(max(dot(n, lampDir), 0.0), 40.0) * ((id > 1.5)? 1.2 : 0.4);
	float3 ambient = lightTone * 0.18 + haze;

	float3 color = albedo * (ambient + (diffuse * lamp).xxx) + (specular * lamp).xxx + emissive;
	color = lerp(color, haze * 4.0, 1.0 - exp(-hit.x * 0.04));
	color += lightTone * glow * params[4].z;

	if(m == 3u) color = n * 0.5 + 0.5;
	return float4((m == 3u)? color : toneMap(color), 1.0);
}
