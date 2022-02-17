SamplerState baseSampler : register(s0);
Texture2D baseTex : register(t0);
SamplerState lineSampler : register(s1);
Texture1D lineTex : register(t1);
SamplerState areaSampler : register(s2);
Texture3D areaTex : register(t2);


struct PS_INPUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0; // may need to increase this to float3
};

float4 main(PS_INPUT input) : SV_TARGET{
	return float4(1.0f, 1.0f, 1.0f, 1.0f); // white color test
}