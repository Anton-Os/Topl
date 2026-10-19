// Planet terrain shared by the planet vertex and pixel stages, mirrored on the CPU by the planetos demo for walking
// params[0].xyz, params[1].xyz, params[2].xyz: rows of the planet rotation (planet frame to world)
// params[0].w radius, params[1].w height scale, params[2].w sea level
// params[3]: x terrain time, y noise frequency, z pulse age in seconds (negative for none), w pulse strength
// params[4]: xyz pulse origin (planet frame), w biome (0 earth, 1 desert, 2 ice, 3 volcanic)
// params[5]: xyz planet center (world), w wave strength

float getRadius(){ return params[0].w; }
float getHeightScale(){ return max(params[1].w, 0.001); }
float getSeaLevel(){ return params[2].w; }

float3 planetToWorld(float3 d){ return float3(dot(params[0].xyz, d), dot(params[1].xyz, d), dot(params[2].xyz, d)); }
float3 worldToPlanet(float3 w){ return params[0].xyz * w.x + params[1].xyz * w.y + params[2].xyz * w.z; }

float valueNoise3(float3 p){
	float3 i = floor(p);
	float3 f = frac(p);
	float3 u = f * f * (3.0 - 2.0 * f);
	float n000 = hash13(i), n100 = hash13(i + float3(1.0, 0.0, 0.0));
	float n010 = hash13(i + float3(0.0, 1.0, 0.0)), n110 = hash13(i + float3(1.0, 1.0, 0.0));
	float n001 = hash13(i + float3(0.0, 0.0, 1.0)), n101 = hash13(i + float3(1.0, 0.0, 1.0));
	float n011 = hash13(i + float3(0.0, 1.0, 1.0)), n111 = hash13(i + float3(1.0, 1.0, 1.0));
	float x00 = lerp(n000, n100, u.x), x10 = lerp(n010, n110, u.x);
	float x01 = lerp(n001, n101, u.x), x11 = lerp(n011, n111, u.x);
	return lerp(lerp(x00, x10, u.y), lerp(x01, x11, u.y), u.z);
}

float3 rotateOctave(float3 p){ return float3(0.80 * p.y + 0.60 * p.z, -0.80 * p.x + 0.36 * p.y - 0.48 * p.z, -0.60 * p.x - 0.48 * p.y + 0.64 * p.z); }

float fbm3(float3 p){
	float total = 0.0, amp = 0.5, norm = 0.0;
	[unroll] for(int o = 0; o < 5; o++){
		total += valueNoise3(p) * amp;
		norm += amp;
		p = rotateOctave(p) * 2.03;
		amp *= 0.5;
	}
	return total / norm;
}

float ridged3(float3 p){
	float total = 0.0, amp = 0.5, norm = 0.0;
	[unroll] for(int o = 0; o < 4; o++){
		float n = 1.0 - abs(valueNoise3(p) * 2.0 - 1.0);
		total += n * n * amp;
		norm += amp;
		p = rotateOctave(p) * 2.11;
		amp *= 0.5;
	}
	return total / norm;
}

float planetTerrain(float3 dir){ // height above the base radius in world units, dir is a unit vector in the planet frame
	float t = params[3].x;
	float f = params[3].y;
	float3 drift = float3(sin(t * 0.11), cos(t * 0.07), sin(t * 0.05 + 1.3)) * 0.35; // continents slowly reshape
	float continents = fbm3(dir * f + drift) * 2.0 - 1.0;
	float mountains = ridged3(dir * f * 3.1 - drift * 1.7);
	float h = (continents * 1.4 + mountains * smoothstep(0.0, 0.3, continents) * 1.1) * getHeightScale();

	if(params[3].z >= 0.0){ // quake ring travelling outward from the pulse origin
		float angle = acos(clamp(dot(dir, params[4].xyz), -1.0, 1.0));
		float front = (angle - params[3].z * 0.25) * getRadius() / 6.0;
		h += exp(-front * front) * params[3].w * exp(-params[3].z * 0.3);
	}
	return h;
}

float3 planetTangent(float3 dir){ return normalize(cross(dir, (abs(dir.y) < 0.99)? float3(0.0, 1.0, 0.0) : float3(1.0, 0.0, 0.0))); }
