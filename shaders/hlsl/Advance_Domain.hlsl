// Values

struct DS_INPUT {
	float4 pos : SV_POSITION;
};

struct PATCH_OUTPUT {
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct DS_OUTPUT {
	float4 pos : SV_POSITION;
};

// Main

[domain("tri")]

DS_OUTPUT main(PATCH_OUTPUT input, float3 domainCoord : SV_DomainLocation, const OutputPatch<DS_INPUT, 3> patch) {
	DS_OUTPUT output;

	output.pos = domainCoord[0] * patch[0].pos + domainCoord[1] * patch[1].pos + domainCoord[2] * patch[2].pos;

	return output;
}