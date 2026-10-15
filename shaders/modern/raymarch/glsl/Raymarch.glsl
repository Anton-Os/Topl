// Signed distance scene for the raymarch pipeline

#define MARCH_STEPS 128
#define MARCH_FAR 120.0
#define MARCH_EPSILON 0.0015

float sdSphere(vec3 p, float r){ return length(p) - r; }

float sdBox(vec3 p, vec3 b, float r){
	vec3 q = abs(p) - b;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

float sdTorus(vec3 p, vec2 t){
	vec2 q = vec2(length(p.xz) - t.x, p.y);
	return length(q) - t.y;
}

float smoothUnion(float a, float b, float k){
	float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
	return mix(b, a, h) - k * h * (1.0 - h);
}

vec2 mapScene(vec3 p){ // x distance, y material id
	float t = getTime();
	vec2 res = vec2(p.y + 0.0, 0.0); // ground plane

	// metaball cluster
	float blob = sdSphere(p - vec3(0.0, 1.6 + sin(t) * 0.3, 6.0), 0.9);
	for(int i = 0; i < 3; i++){
		float a = t * (0.7 + float(i) * 0.25) + float(i) * 2.094;
		blob = smoothUnion(blob, sdSphere(p - vec3(cos(a) * 1.6, 1.6 + sin(a * 1.3) * 0.6, 6.0 + sin(a) * 1.6), 0.5), 0.6);
	}
	if(blob < res.x) res = vec2(blob, 1.0);

	// spinning torus
	vec3 tp = p - vec3(-4.5, 1.4, 9.0);
	tp = rotateX(rotateZ(tp, t * 0.6), t * 0.4);
	float torus = sdTorus(tp, vec2(1.1, 0.32));
	if(torus < res.x) res = vec2(torus, 2.0);

	// rounded box
	vec3 bp = rotateY(p - vec3(4.5, 1.1, 9.0), t * 0.5);
	float box = sdBox(bp, vec3(0.8), 0.15);
	if(box < res.x) res = vec2(box, 3.0);

	// repeating pillars
	vec3 rp = p;
	rp.xz = rp.xz - 8.0 * floor(rp.xz / 8.0 + 0.5);
	float pillar = sdBox(rp - vec3(0.0, 2.0, 0.0), vec3(0.35, 2.0, 0.35), 0.05);
	if(length(p.xz - vec2(0.0, 7.0)) > 9.0 && pillar < res.x) res = vec2(pillar, 4.0);

	return res;
}

vec3 getSceneNormal(vec3 p){
	const vec2 e = vec2(1.0, -1.0) * 0.0015;
	return normalize(
		e.xyy * mapScene(p + e.xyy).x + e.yyx * mapScene(p + e.yyx).x +
		e.yxy * mapScene(p + e.yxy).x + e.xxx * mapScene(p + e.xxx).x
	);
}

float getSoftShadow(vec3 ro, vec3 rd){
	float res = 1.0, t = 0.03;
	for(int i = 0; i < 48; i++){
		float h = mapScene(ro + rd * t).x;
		res = min(res, 10.0 * h / t);
		t += clamp(h, 0.02, 0.5);
		if(res < 0.002 || t > 25.0) break;
	}
	return clamp(res, 0.0, 1.0);
}

float getOcclusion(vec3 p, vec3 n){
	float occ = 0.0, weight = 1.0;
	for(int i = 1; i <= 5; i++){
		float d = 0.06 * float(i);
		occ += (d - mapScene(p + n * d).x) * weight;
		weight *= 0.7;
	}
	return clamp(1.0 - occ * 2.5, 0.0, 1.0);
}

vec3 getMaterial(float id, vec3 p){
	if(id < 0.5){ // checker ground
		vec2 c = floor(p.xz);
		float checker = mod(c.x + c.y, 2.0);
		return mix(vec3(0.18), vec3(0.42), checker);
	}
	if(id < 1.5) return hsv2rgb(vec3(fract(getTime() * 0.05 + p.y * 0.1), 0.6, 0.9));
	if(id < 2.5) return vec3(0.95, 0.65, 0.2);
	if(id < 3.5) return vec3(0.25, 0.55, 0.95);
	return vec3(0.75, 0.72, 0.68);
}
