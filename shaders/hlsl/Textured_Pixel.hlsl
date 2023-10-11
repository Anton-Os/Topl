// Values

Texture2D baseTex : register(t0);
SamplerState baseSampler : register(s0);

Texture2D tex1 : register(t1); SamplerState sampler1 : register(s1);
Texture2D tex2 : register(t2); SamplerState sampler2 : register(s2);
Texture2D tex3 : register(t3); SamplerState sampler3 : register(s3);
Texture2D tex4 : register(t4); SamplerState sampler4 : register(s4);
Texture2D tex5 : register(t5); SamplerState sampler5 : register(s5);

Texture3D areaTex : register(t6);
SamplerState areaSampler : register(s6);

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Functions

float4 switchRB(float4 color){ // switch red and blue color values
	float t = color[0];
	color[0] = color[2];
	color[2] = t;
	return color;
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float4 color = baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y));
	// float4 color = areaTex.Sample(areaSampler, float3(input.texcoord.x, input.texcoord.y, input.texcoord.z));
	color = switchRB(color);
	return color;
}