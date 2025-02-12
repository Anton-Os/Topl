// Values

#define IGNORE_INPUTS
#define INCLUDE_EXTBLOCK

#define STAGE_TESS_EVAL

#include "../Common.hlsl"

#include "../Extended.hlsl"

struct HS_INPUT {
	float4 pos : SV_POSITION;
};

struct PATCH_OUTPUT {
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct HS_OUTPUT {
	float4 pos : SV_POSITION;
};

// Functions

PATCH_OUTPUT PatchFunction(InputPatch<HS_INPUT, 3> inputPatch, uint patchID : SV_PrimitiveID) {
	PATCH_OUTPUT patchOutput;

	patchOutput.edges[0] = 0.5f;
	patchOutput.edges[1] = 0.5f;
	patchOutput.edges[2] = 0.5f;

	patchOutput.inside = 0.5f;

	return patchOutput;
}

// Main

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchFunction")]

HS_OUTPUT main(InputPatch<HS_INPUT, 3> patch, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID) {
	HS_OUTPUT output;

	output.pos = patch[pointID].pos;

	return output;
}