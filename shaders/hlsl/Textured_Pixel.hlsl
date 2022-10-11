// Values

Texture2D baseTex : register(t0);
SamplerState baseSampler : register(s0);

// SamplerState areaSampler : register(s2);
// Texture3D areaTex : register(t2);

SamplerState customSampler {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
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
	/* uint height;
	uint width;
	baseTex.GetDimensions(width, height); */

	float4 color = baseTex.Sample(baseSampler, input.texcoord);
	color = switchRB(color);
	return color;
}