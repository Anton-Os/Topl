#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#ifdef INCLUDE_BLOCK
cbuffer CONST_BLOCK : register(b0) {
	// uint actorID;
	float3 offset;
	float3 rotation;
	float3 scale;
}
#endif

#ifdef INCLUDE_SCENEBLOCK
cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;
}
#endif

#ifdef INCLUDE_EXTBLOCK
#define MAX_INSTANCES 26

cbuffer CONST_EXT_BLOCK : register(b2) { // TODO: Change this to time and audio block
	// double time; // duration in milliseconds
	uint vertCount; // count for vertices
	uint instCount; // count for rendering instances
	uint drawMode; // draw mode cooresponding to primitive
	uint tessLevel; // levels of tesselation

    float4x4 instanceData[MAX_INSTANCES];
	// Include audio data including pitch and volume at different hertz ranges
}
#endif

#ifdef INCLUDE_DATASTREAM
// ByteAddressBuffer feedIn : register(t9); // Buffer feedIn : register(b3) { float3[]; }
RWByteAddressBuffer feedOut : register(u0); // RWBuffer feedOut : register(b4) { float3[]; }
#endif

#define TRACER_STEPS 16
#define TRACER_PATHS 16



#ifdef INCLUDE_TEXTURES

#ifndef SLICE
#define SLICE 0.0f
#endif

Texture2D baseTex : register(t0);
SamplerState baseSampler : register(s0);

Texture2D tex1 : register(t1); SamplerState sampler1 : register(s1);
Texture2D tex2 : register(t2); SamplerState sampler2 : register(s2);
Texture2D tex3 : register(t3); SamplerState sampler3 : register(s3);
Texture2D tex4 : register(t4); SamplerState sampler4 : register(s4);
Texture2D tex5 : register(t5); SamplerState sampler5 : register(s5);
Texture2D tex6 : register(t6); SamplerState sampler6 : register(s6);
Texture2D tex7 : register(t7); SamplerState sampler7 : register(s7);

Texture3D areaTex : register(t8);
SamplerState areaSampler : register(s8);
#endif

#define TWO_PI 6.283185307
#define PI 3.141592653
#define HALF_PI 1.570796327
#define COLOR_INC 0.00390625

// Functions

uint4 getModes(int mode){
	return uint4(abs(mode) % 10, (abs(mode) - (abs(mode) % 10)) / 10, (abs(mode) - (abs(mode) % 100)) / 100, (abs(mode) - (abs(mode) % 1000)) / 1000);
}

bool getInBound(float val, float cmp){ return val > -cmp && val < cmp; }

bool nonZeroMatrix(float4x4 targetMatrix){
	for(uint m = 0; m < 4; m++) 
		for(uint n = 0; n < 4; n++)
			if(targetMatrix[m][n] != 0.0) return true;
	return false;
}

      float3 range_effect(float3 color){ // maps color to +- range
	return (color - float3(0.5F, 0.5F, 0.5F)) * 2;
}

#ifdef INCLUDE_TEXTURES

float4 modalTex(uint sampleMode, float3 texcoords){
    if(abs(sampleMode) % 10 == 1) return tex1.Sample(sampler1, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 10 == 2) return tex2.Sample(sampler2, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 10 == 3) return tex3.Sample(sampler3, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 10 == 4) return tex4.Sample(sampler4, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 10 == 5) return tex5.Sample(sampler5, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 10 == 6) return tex6.Sample(sampler6, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 10 == 7) return tex7.Sample(sampler7, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 10 == 8) return areaTex.Sample(areaSampler, texcoords);
	else if(abs(sampleMode) % 10 == 9) return areaTex.Sample(areaSampler, float3(texcoords.x, texcoords.y, SLICE));
	else return baseTex.Sample(baseSampler, float2(texcoords.x, texcoords.y));
}

float4 antialias2D(float2 coords, Texture2D tex, SamplerState samp, float antialiasArea, float antialiasSteps){
	if(antialiasArea == 0.0 || antialiasSteps == 0) return tex.Sample(samp, coords);
	else { // antialiasing algorithm
		float4 texColor = tex.Sample(samp, coords);
		for(uint a = 0; a < antialiasSteps; a++){
			float f = (antialiasArea / antialiasSteps) * (a + 1);
			float4 nebrTexColors[8] = {
				tex.Sample(samp, coords + float2(f, 0.0)), tex.Sample(samp, coords + float2(-f, 0.0)), // left and right
				tex.Sample(samp, coords + float2(0.0, f)), tex.Sample(samp, coords + float2(0.0, -f)), // top and bottom
				tex.Sample(samp, coords + float2(f, f)), tex.Sample(samp, coords + float2(-f, -f)), // top right and bottom left
				tex.Sample(samp, coords + float2(-f, f)), tex.Sample(samp, coords + float2(f, -f)) // top left and bottom right
			};
			for(uint n = 0; n < 8; n++) texColor += nebrTexColors[n]; // total
			texColor *= 1.0 / 8; // average
		}
		return texColor;
	}
}

float4 antialias3D(float3 coords, Texture3D tex, SamplerState samp, float antialiasArea, float antialiasSteps){
	if(antialiasArea == 0.0 || antialiasSteps == 0) return tex.Sample(samp, coords);
	else {
		float4 texColor = tex.Sample(samp, coords);
		for(uint a = 0; a < antialiasSteps; a++){
			float f = (antialiasArea / antialiasSteps) * (a + 1);
			for(uint l = 0; l < 3; l++){
				float d = -f + (f * l);
				float4 nebrTexColors[9] = {
					tex.Sample(samp, coords + float3(0.0, 0.0, d)),
					tex.Sample(samp, coords + float3(f, 0.0, d)), tex.Sample(samp, coords + float3(-f, 0.0, d)), // left and right
					tex.Sample(samp, coords + float3(0.0, f, d)), tex.Sample(samp, coords + float3(0.0, -f, d)), // top and bottom
					tex.Sample(samp, coords + float3(f, f, d)), tex.Sample(samp, coords + float3(-f, -f, d)), // top right and bottom left
					tex.Sample(samp, coords + float3(-f, f, d)), tex.Sample(samp, coords + float3(f, -f, d)) // top left and bottom right
				};
				for(uint n = 0; n < 9; n++) texColor += nebrTexColors[n]; // total
				texColor *= 1.0 / 9; // average
			}
		}
		return texColor;
	}
}

#endif

float3 getRandColor(uint seed){
	double3 randColor = double3(double(seed) * 0.722433, double(seed) * 0.423512, double(seed) * 0.324561);

	for(uint iter = 0; iter < 1; iter++){
		randColor.x *= randColor.z * 3.527719;
		randColor.y *= randColor.x * 4.342534;
		randColor.z *= randColor.y * 2.834565;
	}

	return float3(randColor.x - floor(randColor.x), randColor.y - floor(randColor.y), randColor.z - floor(randColor.z));
}

float4 getStepColor(int id){
	float attenuation = floor(id / 6.0) * COLOR_INC;

	if(id % 6 == 0) return float4(1.0 - attenuation, 0.0, 0.0, 1.0); // red
	else if (id % 6 == 1) return float4(0.0, 1.0 - attenuation, 0.0, 1.0); // green
	else if (id % 6 == 2) return float4(0.0, 0.0, 1.0 - attenuation, 1.0); // blue
	else if (id % 6 == 3) return float4(1.0 - attenuation, 1.0 - attenuation, 0.0, 1.0); // yellow
	else if (id % 6 == 4) return float4(0.0, 1.0 - attenuation, 1.0 - attenuation, 1.0); // cyan
	else if (id % 6 == 5) return float4(1.0 - attenuation, 0.0, 1.0 - attenuation, 1.0); // magenta
	else return float4(1.0 - attenuation, 1.0 - attenuation, 1.0 - attenuation, 1.0); // white
}

float getSpecular(float4 camera, float3 vertex, float focus) { // Custom Function
	float intensity = dot(normalize(float3(camera.x, camera.y, camera.z)), normalize(vertex)) * focus;
	return max(pow(intensity, 3), 0);
}

float getDiffuse(float3 light, float3 vertex) {
	float intensity = dot(normalize(light), normalize(vertex));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}


// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	float3 skyLight[2]; // position and value
	float3 flashLight[2]; // position and value
	float3 lampLight[2]; // position and value
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 vertex_pos : POSITION;
	float3 normal : NORMAL;
	float3 texcoord : TEXCOORD;
};

uint getLightCount(uint mode){ // used for determining how many lights to combine
    uint count = 1;
    if(mode >= 3) count = 2;
    if(mode >= 6) count = 3;
    return count;
}


// Main

float4 main(PS_INPUT input) : SV_TARGET{
    uint4 modes = getModes(mode);
	uint intensity = modes[3] + 1;

	float3 target;
	if(mode >= 0) target = input.normal; 
	else target = input.vertex_pos;
	
    float3 lights[3][2];
    if(modes[1] % 3 == 1){ lights[0] = flashLight; lights[1] = lampLight; lights[2] = skyLight; }
    else if(modes[1] % 3 == 2){ lights[0] = lampLight; lights[1] = skyLight; lights[2] = flashLight; }
    else{ lights[0] = skyLight; lights[1] = flashLight; lights[2] = lampLight; }

	float cam_dist = distance(target, float3(cam_pos.x, cam_pos.y, cam_pos.z));
#ifdef INCLUDE_TEXTURES
	float4 texTargets[3] = { modalTex(modes[2], input.texcoord), modalTex(modes[2] + 1, input.texcoord), modalTex(modes[2] + 2, input.texcoord) };
	float3 ambient = (texTargets[0].rgb + getAmbient_sampled(lights, intensity)) * 0.5;
	float3 diffuse = (texTargets[1].rgb + getDiffuse_sampled(lights, target, intensity)) * 0.5;
	float3 specular = (texTargets[2].rgb + getSpecular_sampled(lights, target, intensity)) * 0.5;
#else
	float3 ambient = getAmbient_flat(lights, intensity);
	float3 diffuse = getDiffuse_flat(lights, target, intensity);
	float3 specular = getSpecular_flat(lights, target, intensity);
#endif

	if(modes[0] == 1) return float4(ambient, 1.0f);
	else if(modes[0] == 2) return float4(diffuse, 1.0f);
	else if(modes[0] == 3) return float4(specular, 1.0f);
	/* else if(modes[0] == 4) return float4(ambient + (lights[0][1] * dot(normalize(float3(cam_pos.x, cam_pos.y, cam_pos.z)), normalize(target))), 1.0);
	else if(modes[0] == 5) return float4(ambient.r + pow(specular.r, 1.0 / diffuse.r), ambient.g + pow(specular.g, 1.0 / diffuse.g), ambient.b + pow(specular.b, 1.0 / diffuse.b), 1.0);
	else if(modes[0] == 6) return float4(float3(cam_dist, cam_dist, cam_dist) * lights[0][1], 1.0f);
	else if(modes[0] == 7) return float4(length(lights[0][1] - target), length(lights[1][1] - target), length(lights[2][1] - target), 1.0f);
	// else if(modes[0] == 7) return float4(ambient + (cam_dist * diffuse) + specular, 1.0f);
	else if(modes[0] == 8) return float4(ambient + (distance(target, lights[0][0]) * diffuse) + (specular * cross(target, lights[0][0])), 1.0f);
	// else if(modes[0] == 8) return float4(lights[0][1] * normalize(cross(lights[0][0] - float3(cam_pos.x, cam_pos.y, cam_pos.z), target)), 1.0);
	else if(modes[0] == 9) return float4(ambient + float3(cos(1.0 / (diffuse.r * specular.r)), sin(1.0 / (specular.g * diffuse.g)), tan(diffuse.b - specular.b)), 1.0); */
	else if(modes[0] == 4) return float4(ambient * diffuse * specular, 1.0f);
	else if(modes[0] == 5) return float4(ambient * (diffuse + specular), 1.0f);
	else if(modes[0] == 6) return float4(ambient + (diffuse * specular), 1.0f);
	else if(modes[0] == 7) return float4(ambient + (diffuse / specular), 1.0f);
	else if(modes[0] == 6) return float4(smoothstep(ambient, diffuse, specular), 1.0f);
	else if(modes[0] == 7) return float4(smoothstep(ambient, specular, diffuse), 1.0f);
	else if(modes[0] == 8) return float4(ambient * cross(diffuse, specular), 1.0f);
	else if(modes[0] == 9) return float4(ambient + cross(diffuse, specular), 1.0f);
	else return float4(ambient + diffuse + specular, 1.0);
}