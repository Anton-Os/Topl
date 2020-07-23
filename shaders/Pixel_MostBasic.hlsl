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

	return baseTex.Sample(baseSampler, input.texcoord);

	/* if(colorVec.a < 0.5f) discard;
	else return colorVec; */
}