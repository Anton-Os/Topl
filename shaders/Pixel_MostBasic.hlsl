Texture2D baseTex : register(t0);
SamplerState baseSampler : register(s0);

SamplerState customSampler {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET{

	uint height;
	uint width;
	baseTex.GetDimensions(width, height);

	uint4 colorVec = baseTex.Sample(customSampler, input.texcoord);

	if (colorVec[3] == 0 && colorVec[2] == 0 && colorVec[1] == 0 && colorVec[0] == 0)
		return float4(0.8f, 0.3f, 0.4f, 1.0f);
	else
		return float4(0.3f, 0.8f, 0.4f, 1.0f);
}