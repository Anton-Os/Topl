// Sky effects: clouds, stars and aurora layered on top of getSkyColor()

float2 getSkyPlane(float3 dir, float height){ return dir.xz * (height / max(dir.y, 0.02)); } // projects ray onto a cloud layer

float3 getCloudySky(float3 dir){
	float3 sky = getSkyColor(dir);
	if(dir.y <= 0.0) return sky;

	float2 uv = getSkyPlane(dir, 1.0) * 0.6 + float2(getTime() * 0.02, getTime() * 0.008);
	float density = smoothstep(0.45, 0.8, fbm(uv * 1.5));
	float shade = 0.65 + 0.35 * fbm(uv * 3.0 + 4.0);
	float3 cloud = lerp(fog_color.rgb, light_color.rgb, 0.6) * shade;
	return lerp(sky, cloud, density * smoothstep(0.0, 0.25, dir.y));
}

float3 getNightSky(float3 dir){
	float3 sky = getSkyColor(dir) * 0.35;
	float3 cell = floor(dir * 180.0);
	float star = hash13(cell);
	float twinkle = 0.6 + 0.4 * sin(getTime() * 3.0 + star * 100.0);
	if(star > 0.996 && dir.y > -0.05) sky += float3(0.9, 0.95, 1.0) * twinkle * ((star - 0.996) * 250.0);
	return sky;
}

float3 getAurora(float3 dir){
	if(dir.y <= 0.0) return float3(0.0, 0.0, 0.0);
	float2 uv = getSkyPlane(dir, 1.0);
	float bands = 0.0;
	[unroll] for(int i = 0; i < 3; i++){
		float fi = float(i);
		float wave = sin(uv.x * (1.2 + fi * 0.4) + getTime() * (0.25 + fi * 0.1) + fbm(uv * 0.7 + fi) * 4.0);
		bands += smoothstep(0.08, 0.0, abs(uv.y * 0.5 - 0.6 - fi * 0.35 - wave * 0.25)) * (0.6 - fi * 0.15);
	}
	float3 tint = lerp(float3(0.1, 1.0, 0.55), float3(0.6, 0.2, 1.0), saturate(dir.y * 1.5));
	return tint * bands * smoothstep(0.0, 0.3, dir.y);
}
