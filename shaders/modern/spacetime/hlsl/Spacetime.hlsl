// Spacetime curvature, mirrored on the CPU by Spacetime_VertexShader::getWell()
// params[0..3]: xyz mass position, w mass
// params[4]: x gravity scale, y softening, z wave amplitude, w wave frequency

#define SPACETIME_MASSES 4

float getWell(float2 xz){ // depth of the gravity well under a point
	float gravity = (params[4].x > 0.0)? params[4].x : 1.0;
	float soft = (params[4].y > 0.0)? params[4].y : 1.0;
	float depth = 0.0;
	[unroll] for(int i = 0; i < SPACETIME_MASSES; i++){
		float2 d = xz - params[i].xz;
		depth += gravity * params[i].w / sqrt(dot(d, d) + soft * soft);
	}
	return depth;
}

float getRipple(float2 xz){ // gravitational waves radiating from the heaviest mass
	float amp = params[4].z;
	if(amp <= 0.0) return 0.0;
	float freq = (params[4].w > 0.0)? params[4].w : 1.2;
	float dist = length(xz - params[0].xz);
	return sin(dist * freq - getTime() * 3.0) * amp / (1.0 + dist * 0.35);
}
