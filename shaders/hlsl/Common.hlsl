#ifdef INCLUDE_BLOCK
cbuffer CONST_BLOCK : register(b0) {
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
cbuffer CONST_EXT_BLOCK : register(b2) {
	uint vertCount; // count for vertices
	uint instCount; // count for rendering instances
	uint drawMode; // draw mode cooresponding to primitive
	uint tessLevel; // levels of tesselation
	// float3 meshOrigin; // origin point for mesh
}
#endif

#define STEP_ATTENUATION 0.001

#ifdef INCLUDE_TEXTURES
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

#ifndef IGNORE_INPUTS
struct VS_INPUT {
	float4 pos : POSITION;
	float3 texcoord : TEXCOORD;
	// float3x3 instanceData : INSTANCE;
};
#endif

// Functions

uint4 getModes(uint mode){
	return uint4(mode % 10, (mode - (mode % 10)) / 10, (mode - (mode % 100)) / 100, (mode - (mode % 1000)) / 1000);
}

float4 getRandColor(float4 seedColor){
	float4 randColor = (seedColor + float4(0.569, 0.853, 0.265, 1.0)) * float4(881.225, 111.159, 454.823, 1.0);

	return float4(randColor.x - floor(randColor.x), randColor.y - floor(randColor.y), randColor.z - floor(randColor.z), 1.0);
}

float4 getStepColor(int id){
	float attenuation = floor(id / 6.0) * STEP_ATTENUATION;

	if(id % 6 == 0) return float4(1.0 - attenuation, 0.0, 0.0, 1.0); // red
	else if (id % 6 == 1) return float4(0.0, 1.0 - attenuation, 0.0, 1.0); // green
	else if (id % 6 == 2) return float4(0.0, 0.0, 1.0 - attenuation, 1.0); // blue
	else if (id % 6 == 3) return float4(1.0 - attenuation, 1.0 - attenuation, 0.0, 1.0); // yellow
	else if (id % 6 == 4) return float4(0.0, 1.0 - attenuation, 1.0 - attenuation, 1.0); // cyan
	else if (id % 6 == 5) return float4(1.0 - attenuation, 0.0, 1.0 - attenuation, 1.0); // magenta
	else return float4(1.0 - attenuation, 1.0 - attenuation, 1.0 - attenuation, 1.0); // white
}

float3x3 getRotMatrix(float3 angles) {
	float3x3 zRotMatrix = { // Roll
		cos(angles.x), sin(angles.x), 0,
		-1 * sin(angles.x), cos(angles.x), 0,
		0, 0, 1
	};

	float3x3 xRotMatrix = { // Pitch
		1, 0, 0,
		0, cos(angles.y), sin(angles.y),
		0, -sin(angles.y), cos(angles.y)
	};

	float3x3 yRotMatrix = { // Yaw
		cos(angles.z), 0, sin(angles.z),
		0, 1, 0,
		-1.0 * sin(angles.z), 0, cos(angles.z)
	};

	return mul(mul(zRotMatrix, yRotMatrix), xRotMatrix);
}

float4x4 getCamMatrix(float4 cPos, float3 angles) { // camera postion and relative look position
	float4x4 camMatrix = {
		cos(angles.z) * cos(angles.x), -sin(angles.x), sin(angles.z), -cPos.x,
		sin(angles.x), cos(angles.x) * cos(angles.y), sin(angles.y), -cPos.y,
		-1.0 * sin(angles.z), -sin(angles.y), cos(angles.y) * cos(angles.z), -cPos.z,
		0, 0, 0, 1
	};

	return camMatrix;
}

float4 color_correct(float4 color){ // switch red and blue color values
	float t = color[0];
	color[0] = color[2]; color[2] = t;
	return color;
}

// TODO: Include other helper functions
