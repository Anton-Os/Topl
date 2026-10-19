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

float sdBoxSharp(float3 p, float3 b){
	float3 q = abs(p) - b;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float2 deMandelbulb(float3 p){
	float3 z = p;
	float dr = 1.0, r = length(z), trap = 1e10;
	float power = (getShape() > 1.0)? getShape() : 8.0;
	[loop] for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= getIterations()) break;
		r = length(z);
		if(r > 2.0) break;
		r = max(r, 1e-6);
		float theta = acos(clamp(z.z / r, -1.0, 1.0)) * power;
		float phi = atan2(z.y, z.x) * power;
		dr = pow(r, power - 1.0) * power * dr + 1.0;
		z = pow(r, power) * float3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta)) + p;
		trap = min(trap, dot(z, z));
	}
	r = max(r, 1e-6);
	return float2(0.5 * log(r) * r / dr, trap);
}

float2 deMenger(float3 p){
	float d = sdBoxSharp(p, float3(1.0, 1.0, 1.0));
	float s = 1.0, trap = 1e10;
	[loop] for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= min(getIterations(), 7)) break;
		float3 q = p * s + params[2].w * float(i);
		float3 a = q - 2.0 * floor(q / 2.0) - 1.0; // GLSL style mod
		s *= 3.0;
		float3 r = abs(1.0 - 3.0 * abs(a));
		float da = max(r.x, r.y), db = max(r.y, r.z), dc = max(r.z, r.x);
		float c = (min(da, min(db, dc)) - 1.0) / s;
		d = max(d, c);
		trap = min(trap, dot(a, a) * (1.0 + float(i)));
	}
	return float2(d, trap);
}

float2 deSierpinski(float3 z){
	float scale = 2.0, trap = 1e10;
	float3 offset = (1.0 + params[2].w * 0.5).xxx;
	int n = 0;
	[loop] for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= getIterations()) break;
		if(z.x + z.y < 0.0) z.xy = -z.yx;
		if(z.x + z.z < 0.0) z.xz = -z.zx;
		if(z.y + z.z < 0.0) z.zy = -z.yz;
		z = z * scale - offset * (scale - 1.0);
		trap = min(trap, dot(z, z));
		n++;
	}
	return float2((length(z) - 1.5) * pow(scale, -float(n)), trap);
}

float2 deMandelbox(float3 p){
	float scale = (abs(getShape()) > 0.5)? getShape() : -1.8;
	float3 z = p;
	float dr = 1.0, trap = 1e10;
	[loop] for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= getIterations()) break;
		z = clamp(z, -1.0, 1.0) * 2.0 - z; // box fold
		float r2 = dot(z, z);
		if(r2 < 0.25){ z *= 4.0; dr *= 4.0; } // sphere fold
		else if(r2 < 1.0){ z /= r2; dr /= r2; }
		z = z * scale + p;
		dr = dr * abs(scale) + 1.0;
		trap = min(trap, r2);
	}
	return float2(length(z) / abs(dr), trap);
}

float4 quatSquare(float4 a){ return float4(a.x * a.x - dot(a.yzw, a.yzw), 2.0 * a.x * a.yzw); }

float2 deJulia(float3 p){
	float4 z = float4(p, 0.0);
	float4 c = params[3];
	float md2 = 1.0, mz2 = dot(z, z), trap = 1e10;
	[loop] for(int i = 0; i < FRACTAL_MAX_ITER; i++){
		if(i >= getIterations()) break;
		md2 *= 4.0 * mz2;
		z = quatSquare(z) + c;
		mz2 = dot(z, z);
		trap = min(trap, mz2);
		if(mz2 > 4.0) break;
	}
	return float2(0.25 * sqrt(mz2 / max(md2, 1e-10)) * log(max(mz2, 1e-10)), trap);
}

float3 toFractal(float3 p){ return rotateX(rotateY(p / getWorldScale(), params[2].x), params[2].y); } // rotations keep distances

float2 mapFractal(float3 p){
	float3 q = toFractal(p);
	int f = getFractal();
	float2 res;
	if(f == 1) res = deMenger(q);
	else if(f == 2) res = deSierpinski(q);
	else if(f == 3) res = deMandelbox(q);
	else if(f == 4) res = deJulia(q);
	else res = deMandelbulb(q);
	return float2(res.x * getWorldScale(), res.y);
}

float3 getFractalNormal(float3 p, float eps){
	const float2 k = float2(1.0, -1.0);
	return normalize(
		k.xyy * mapFractal(p + k.xyy * eps).x + k.yyx * mapFractal(p + k.yyx * eps).x +
		k.yxy * mapFractal(p + k.yxy * eps).x + k.xxx * mapFractal(p + k.xxx * eps).x
	);
}

float getFractalShadow(float3 ro, float3 rd, float start){
	float res = 1.0, t = start;
	[loop] for(int i = 0; i < 40; i++){
		float h = mapFractal(ro + rd * t).x;
		res = min(res, 8.0 * h / t);
		t += clamp(h, start * 0.5, getWorldScale() * 0.2);
		if(res < 0.002 || t > getBoundRadius() * 2.0) break;
	}
	return saturate(res);
}

float3 getFractalPalette(float t){ // cosine palette
	float3 a = float3(0.55, 0.5, 0.5), b = float3(0.45, 0.45, 0.5), c = float3(1.0, 1.0, 1.0), d = float3(0.0, 0.15, 0.3);
	return a + b * cos(TWO_PI * (c * t + d + params[1].x));
}

float2 hitBounds(float3 ro, float3 rd, float radius){ // near and far distances through the bounding sphere
	float b = dot(ro, rd);
	float c = dot(ro, ro) - radius * radius;
	float h = b * b - c;
	if(h < 0.0) return float2(-1.0, -1.0);
	h = sqrt(h);
	return float2(max(-b - h, 0.0), -b + h);
}
