// Values

Texture2D baseTex : register(t0);
SamplerState baseSampler : register(s0);

Texture2D tex1 : register(t1); SamplerState sampler1 : register(s1); // height?
Texture2D tex2 : register(t2); SamplerState sampler2 : register(s2); // roughness?
Texture2D tex3 : register(t3); SamplerState sampler3 : register(s3); // opacity?
Texture2D tex4 : register(t4); SamplerState sampler4 : register(s4); // enviornment?
Texture2D tex5 : register(t5); SamplerState sampler5 : register(s5); // shadow?
Texture2D tex6 : register(t6); SamplerState sampler6 : register(s6); // illumination?
Texture2D tex7 : register(t7); SamplerState sampler7 : register(s7); // testing?

Texture3D areaTex : register(t8);
SamplerState areaSampler : register(s8);

cbuffer CONST_BLOCK : register(b0) {
	float3 offset;
	float3 rotation;
	float3 scale;
	float3 texScroll; // texture coordinate scrolling
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;
}


struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Functions

float4 color_correct(float4 color){ // switch red and blue color values
	float t = color[0];
	color[0] = color[2]; color[2] = t;
	return color;
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(mode == 8) return color_correct(areaTex.Sample(areaSampler, input.texcoord)); // volumetric texture
	else if(mode != 0){
		if(mode == 1) return color_correct(tex1.Sample(sampler1, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 2) return color_correct(tex2.Sample(sampler2, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 3) return color_correct(tex3.Sample(sampler3, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 4) return color_correct(tex4.Sample(sampler4, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 5) return color_correct(tex5.Sample(sampler5, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 6) return color_correct(tex6.Sample(sampler6, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 7) return color_correct(tex7.Sample(sampler7, float2(input.texcoord.x, input.texcoord.y)));
		else return float4(1.0, 0.0, 0.0, 1.0); // error
	}
	else return color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y))); // base texture
	
}