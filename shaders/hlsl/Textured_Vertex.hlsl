#define INCLUDE_SCENEBLOCK
#define INCLUDE_TEXTURES

#include "Common.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
	float3 offset;
	float3 rotation;
	float3 scale;

	float3 texScroll; // texture coordinate scrolling
	float3 texScale; // texture coordinate scaling
}

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};


// Main

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	float3 angles = mul(getRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x, angles.y, angles.z, 1.0) * float4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	float4x4 cameraMatrix = getCamMatrix(cam_pos, look_pos);
	output.pos = mul(transpose(projMatrix), mul(cameraMatrix, output.pos + float4(offset, 0.0)));
	output.texcoord = (input.texcoord + texScroll) * texScale;

	/* if(abs(mode) >= 10){
		float4 texOffset = baseTex.Sample(baseSampler, float2(output.texcoord.x, output.texcoord.y));
		if(floor(abs(mode) / 10.0) % 10 == 1) texOffset = tex1.Sample(sampler1, float2(output.texcoord.x, output.texcoord.y));
		else if(floor(abs(mode) / 10.0) % 10 == 2) texOffset = tex2.Sample(sampler2, float2(output.texcoord.x, output.texcoord.y));
		else if(floor(abs(mode) / 10.0) % 10 == 3) texOffset = tex3.Sample(sampler3, float2(output.texcoord.x, output.texcoord.y));
		else if(floor(abs(mode) / 10.0) % 10 == 4) texOffset = tex4.Sample(sampler4, float2(output.texcoord.x, output.texcoord.y));
		else if(floor(abs(mode) / 10.0) % 10 == 5) texOffset = tex5.Sample(sampler5, float2(output.texcoord.x, output.texcoord.y));
		else if(floor(abs(mode) / 10.0) % 10 == 6) texOffset = tex6.Sample(sampler6, float2(output.texcoord.x, output.texcoord.y));
		else if(floor(abs(mode) / 10.0) % 10 == 7) texOffset = tex7.Sample(sampler7, float2(output.texcoord.x, output.texcoord.y));
		else if(floor(abs(mode) / 10.0) % 10 == 8) texOffset = areaTex.Sample(areaSampler, output.texcoord);

		if(mode > 0) output.pos.x += texOffset.g * (floor(abs(mode) / 100.0) + 1); else output.pos.x -= texOffset.g * (floor(abs(mode) / 100.0) + 1);
		if(mode > 0) output.pos.y += texOffset.r * (floor(abs(mode) / 100.0) + 1); else output.pos.y -= texOffset.r * (floor(abs(mode) / 100.0) + 1);
		if(mode > 0) output.pos.z += texOffset.b * (floor(abs(mode) / 100.0) + 1); else output.pos.z -= texOffset.b * (floor(abs(mode) / 100.0) + 1);
	} */

	return output;
}