// Interior of a spinning, tilting torus for the tunnel pipeline
// The torus frame is placed so its centerline passes through an anchor point near the camera:
// local = rotateY(transpose(tilt) * (world - anchor) + vec3(major, 0, 0), spin)
// params[0].xyz, params[1].xyz, params[2].xyz: columns of the tilt matrix (torus frame to world)
// params[0].w spin angle, params[1].w major radius, params[2].w tube radius
// params[3].xyz anchor on the centerline in world space, params[3].w rib count
// params[4]: x rib depth, y pipe count, z glow strength, w panel count around the tube
// params[5]: x light pulse speed, y wobble strength, z hue shift, w headlamp strength

#define TUNNEL_STEPS 160
#define TUNNEL_EPSILON 0.0008

float getMajor(){ return params[1].w; }
float getTube(){ return params[2].w; }
float getRibCount(){ return max(params[3].w, 1.0); }
float getPipeCount(){ return max(params[4].y, 1.0); }

vec3 toTorus(vec3 w){
	vec3 d = w - params[3].xyz;
	vec3 l = vec3(dot(params[0].xyz, d), dot(params[1].xyz, d), dot(params[2].xyz, d)) + vec3(getMajor(), 0.0, 0.0);
	return rotateY(l, params[0].w);
}

vec2 getTubeCoords(vec3 l){ // x angle along the torus, y angle around the tube
	vec2 q = vec2(length(l.xz) - getMajor(), l.y);
	return vec2(atan(l.z, l.x), atan(q.y, q.x));
}

float repeatAngle(float a, float sector){ return mod(a + sector * 0.5, sector) - sector * 0.5; }

vec3 mapTunnel(vec3 w, bool isWobble){ // x distance, y material (0 wall, 1 rib, 2 pipe), z distance to the nearest rib
	vec3 l = toTorus(w);
	float ringLen = length(l.xz);
	vec2 q = vec2(ringLen - getMajor(), l.y);
	float lq = length(q);
	float u = atan(l.z, l.x);
	float v = atan(q.y, q.x);

	float wall = getTube() - lq;
	if(isWobble) wall += sin(u * 9.0 + getTime() * 1.5) * sin(v * 5.0 - getTime()) * params[5].y;
	vec3 res = vec3(wall, 0.0, 1e10);

	float sector = TWO_PI / getRibCount(); // rings standing across the tube
	float ribPlane = abs(sin(repeatAngle(u, sector)) * ringLen);
	float rib = max(ribPlane - 0.14, (getTube() - params[4].x) - lq);
	res.z = rib;
	if(rib < res.x) res.xy = vec2(rib, 1.0);

	float tubeSector = TWO_PI / getPipeCount(); // pipes running along the tube
	float vr = repeatAngle(v + tubeSector * 0.5, tubeSector);
	vec2 qr = lq * vec2(cos(vr), sin(vr));
	float pipe = length(qr - vec2(getTube() - 0.42, 0.0)) - 0.15;
	if(pipe < res.x) res.xy = vec2(pipe, 2.0);
	return res;
}

vec3 getTunnelNormal(vec3 p, bool isWobble){
	const vec2 e = vec2(1.0, -1.0) * 0.002;
	return normalize(
		e.xyy * mapTunnel(p + e.xyy, isWobble).x + e.yyx * mapTunnel(p + e.yyx, isWobble).x +
		e.yxy * mapTunnel(p + e.yxy, isWobble).x + e.xxx * mapTunnel(p + e.xxx, isWobble).x
	);
}

vec3 getTunnelLight(float u, float hueShift){ // warm strips that drift along the torus
	float pulse = 0.5 + 0.5 * sin(u * getRibCount() * 0.5 - getTime() * params[5].x);
	return hsv2rgb(vec3(fract(0.08 + hueShift), 0.55, 1.0)) * (0.4 + pulse * 0.9);
}
