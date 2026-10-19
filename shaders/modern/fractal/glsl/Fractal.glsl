// Distance estimators for the fractal pipeline, every estimator returns x distance and y orbit trap
// params[0]: x fractal (0 mandelbulb, 1 menger sponge, 2 sierpinski tetrahedron, 3 mandelbox, 4 quaternion julia)
//            y shape value (bulb power or box scale), z iterations, w world scale
// params[1]: x color shift, y glow strength, z detail (epsilon factor), w bounding radius in fractal units
// params[2]: x spin angle, y tilt angle, z shadow strength, w fold offset for the sponge and tetrahedron
// params[3]: julia constant

#define FRACTAL_STEPS 220
#define FRACTAL_MAX_ITER 24

int getFractal(){ return int(params[0].x + 0.5); }
float getShape(){ return params[0].y; }
int getIterations(){ return int(clamp(params[0].z, 1.0, float(FRACTAL_MAX_ITER))); }
float getWorldScale(){ return (params[0].w > 0.0)? params[0].w : 4.0; }
float getBoundRadius(){ return ((params[1].w > 0.0)? params[1].w : 2.0) * getWorldScale(); }
float getDetail(){ return (params[1].z > 0.0)? params[1].z : 1.0; }

float sdBoxSharp(vec3 p, vec3 b){
	vec3 q = abs(p) - b;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

vec2 deMandelbulb(vec3 p){
	vec3 z = p;
	float dr = 1.0, r = length(z), trap = 1e10;
	float power = (getShape() > 1.0)? getShape() : 8.0;
	for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= getIterations()) break;
		r = length(z);
		if(r > 2.0) break;
		r = max(r, 1e-6);
		float theta = acos(clamp(z.z / r, -1.0, 1.0)) * power;
		float phi = atan(z.y, z.x) * power;
		dr = pow(r, power - 1.0) * power * dr + 1.0;
		z = pow(r, power) * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta)) + p;
		trap = min(trap, dot(z, z));
	}
	r = max(r, 1e-6);
	return vec2(0.5 * log(r) * r / dr, trap);
}

vec2 deMenger(vec3 p){
	float d = sdBoxSharp(p, vec3(1.0));
	float s = 1.0, trap = 1e10;
	for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= min(getIterations(), 7)) break;
		vec3 a = mod(p * s + params[2].w * float(i), 2.0) - 1.0;
		s *= 3.0;
		vec3 r = abs(1.0 - 3.0 * abs(a));
		float da = max(r.x, r.y), db = max(r.y, r.z), dc = max(r.z, r.x);
		float c = (min(da, min(db, dc)) - 1.0) / s;
		d = max(d, c);
		trap = min(trap, dot(a, a) * (1.0 + float(i)));
	}
	return vec2(d, trap);
}

vec2 deSierpinski(vec3 z){
	float scale = 2.0, trap = 1e10;
	vec3 offset = vec3(1.0 + params[2].w * 0.5);
	int n = 0;
	for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= getIterations()) break;
		if(z.x + z.y < 0.0) z.xy = -z.yx;
		if(z.x + z.z < 0.0) z.xz = -z.zx;
		if(z.y + z.z < 0.0) z.zy = -z.yz;
		z = z * scale - offset * (scale - 1.0);
		trap = min(trap, dot(z, z));
		n++;
	}
	return vec2((length(z) - 1.5) * pow(scale, -float(n)), trap);
}

vec2 deMandelbox(vec3 p){
	float scale = (abs(getShape()) > 0.5)? getShape() : -1.8;
	vec3 z = p;
	float dr = 1.0, trap = 1e10;
	for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= getIterations()) break;
		z = clamp(z, -1.0, 1.0) * 2.0 - z; // box fold
		float r2 = dot(z, z);
		if(r2 < 0.25){ z *= 4.0; dr *= 4.0; } // sphere fold
		else if(r2 < 1.0){ z /= r2; dr /= r2; }
		z = z * scale + p;
		dr = dr * abs(scale) + 1.0;
		trap = min(trap, r2);
	}
	return vec2(length(z) / abs(dr), trap);
}

vec4 quatSquare(vec4 a){ return vec4(a.x * a.x - dot(a.yzw, a.yzw), 2.0 * a.x * a.yzw); }

vec2 deJulia(vec3 p){
	vec4 z = vec4(p, 0.0);
	vec4 c = params[3];
	float md2 = 1.0, mz2 = dot(z, z), trap = 1e10;
	for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= getIterations()) break;
		md2 *= 4.0 * mz2;
		z = quatSquare(z) + c;
		mz2 = dot(z, z);
		trap = min(trap, mz2);
		if(mz2 > 4.0) break;
	}
	return vec2(0.25 * sqrt(mz2 / max(md2, 1e-10)) * log(max(mz2, 1e-10)), trap);
}

vec3 toFractal(vec3 p){ return rotateX(rotateY(p / getWorldScale(), params[2].x), params[2].y); } // rotations keep distances

vec2 mapFractal(vec3 p){
	vec3 q = toFractal(p);
	int f = getFractal();
	vec2 res;
	if(f == 1) res = deMenger(q);
	else if(f == 2) res = deSierpinski(q);
	else if(f == 3) res = deMandelbox(q);
	else if(f == 4) res = deJulia(q);
	else res = deMandelbulb(q);
	return vec2(res.x * getWorldScale(), res.y);
}

vec3 getFractalNormal(vec3 p, float eps){
	const vec2 k = vec2(1.0, -1.0);
	return normalize(
		k.xyy * mapFractal(p + k.xyy * eps).x + k.yyx * mapFractal(p + k.yyx * eps).x +
		k.yxy * mapFractal(p + k.yxy * eps).x + k.xxx * mapFractal(p + k.xxx * eps).x
	);
}

float getFractalShadow(vec3 ro, vec3 rd, float start){
	float res = 1.0, t = start;
	for(int i = 0; i < 40; i++){
		float h = mapFractal(ro + rd * t).x;
		res = min(res, 8.0 * h / t);
		t += clamp(h, start * 0.5, getWorldScale() * 0.2);
		if(res < 0.002 || t > getBoundRadius() * 2.0) break;
	}
	return clamp(res, 0.0, 1.0);
}

vec3 getFractalPalette(float t){ // cosine palette
	vec3 a = vec3(0.55, 0.5, 0.5), b = vec3(0.45, 0.45, 0.5), c = vec3(1.0, 1.0, 1.0), d = vec3(0.0, 0.15, 0.3);
	return a + b * cos(TWO_PI * (c * t + d + params[1].x));
}

vec2 hitBounds(vec3 ro, vec3 rd, float radius){ // near and far distances through the bounding sphere
	float b = dot(ro, rd);
	float c = dot(ro, ro) - radius * radius;
	float h = b * b - c;
	if(h < 0.0) return vec2(-1.0);
	h = sqrt(h);
	return vec2(max(-b - h, 0.0), -b + h);
}
