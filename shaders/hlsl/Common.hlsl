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

cbuffer CONST_EXT_BLOCK : register(b2) {
	uint vertCount; // count for vertices
	uint instCount; // count for rendering instances
	uint drawMode; // draw mode cooresponding to primitive
	uint tessLevel; // levels of tesselation

    float4x4 instanceData[MAX_INSTANCES];
}
#endif

#ifdef INCLUDE_DATASTREAM
// ByteAddressBuffer feedIn : register(t9); // Buffer feedIn : register(b3) { float3[]; }
RWByteAddressBuffer feedOut : register(u0); // RWBuffer feedOut : register(b4) { float3[]; }
#endif

#define TRACER_STEPS 16
#define TRACER_PATHS 16

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

#define COLOR_INC 0.00390625

// Functions

uint4 getModes(int mode){
	return uint4(abs(mode) % 10, (abs(mode) - (abs(mode) % 10)) / 10, (abs(mode) - (abs(mode) % 100)) / 100, (abs(mode) - (abs(mode) % 1000)) / 1000);
}

bool nonZeroMatrix(float4x4 targetMatrix){
	for(uint m = 0; m < 4; m++) 
		for(uint n = 0; n < 4; n++)
			if(targetMatrix[m][n] != 0.0) return true;
	return false;
}