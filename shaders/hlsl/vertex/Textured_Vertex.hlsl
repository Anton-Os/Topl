#define INCLUDE_SCENEBLOCK
#define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#include "Common.hlsl"

#include "Vertex.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
	float3 offset;
	float3 rotation;
	float3 scale;

	float3 texScroll; // texture coordinate scrolling
	float4 texScale; // texture coordinate scaling

    uint flip;
    float antialiasArea;
    uint antialiasSteps;
	float slice;
}

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};


// Main

VS_OUTPUT main(VS_INPUT input, uint instanceID : SV_InstanceID) {
	VS_OUTPUT output;

	float4 pos = getVertex(input.pos, offset, rotation, float4(scale, 1.0 / cam_pos.w));
	output.texcoord = (input.texcoord - texScroll) * float3(texScale.x, texScale.y, texScale.z);

	if(abs(mode) >= 10){
		float4 texOffset = baseTex.SampleLevel(baseSampler, float2(output.texcoord.x, output.texcoord.y), 0);
		if(floor(abs(mode) / 10.0) % 10 == 1) texOffset = tex1.SampleLevel(sampler1, float2(output.texcoord.x, output.texcoord.y), 0);
		else if(floor(abs(mode) / 10.0) % 10 == 2) texOffset = tex2.SampleLevel(sampler2, float2(output.texcoord.x, output.texcoord.y), 0);
		else if(floor(abs(mode) / 10.0) % 10 == 3) texOffset = tex3.SampleLevel(sampler3, float2(output.texcoord.x, output.texcoord.y), 0);
		else if(floor(abs(mode) / 10.0) % 10 == 4) texOffset = tex4.SampleLevel(sampler4, float2(output.texcoord.x, output.texcoord.y), 0);
		else if(floor(abs(mode) / 10.0) % 10 == 5) texOffset = tex5.SampleLevel(sampler5, float2(output.texcoord.x, output.texcoord.y), 0);
		else if(floor(abs(mode) / 10.0) % 10 == 6) texOffset = tex6.SampleLevel(sampler6, float2(output.texcoord.x, output.texcoord.y), 0);
		else if(floor(abs(mode) / 10.0) % 10 == 7) texOffset = tex7.SampleLevel(sampler7, float2(output.texcoord.x, output.texcoord.y), 0);
		else if(floor(abs(mode) / 10.0) % 10 == 8) texOffset = areaTex.SampleLevel(areaSampler, output.texcoord, 0);

		if(mode >= 0){
			output.texcoord.x += (texOffset.g - 0.5F) * (floor(abs(mode) / 100.0) + 1);
			output.texcoord.y += (texOffset.r - 0.5F) * (floor(abs(mode) / 100.0) + 1);
			output.texcoord.z += (texOffset.b - 0.5F) * (floor(abs(mode) / 100.0) + 1);
		} else {
			output.pos.x *= texOffset.g * (floor(abs(mode) / 100.0) + 1);
			output.pos.y *= texOffset.r * (floor(abs(mode) / 100.0) + 1);
			output.pos.z *= texOffset.b * (floor(abs(mode) / 100.0) + 1);
		}
	}

	output.pos = mul(transpose(projMatrix), mul(getLookAtMatrix(cam_pos, look_pos), pos));
#ifdef INCLUDE_EXTBLOCK
	if(instanceID > 0 && instanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[instanceID])) output.pos = mul(instanceData[instanceID], output.pos); // instanced transform
#endif
	return output;
}