// Values

float3 positions[3] = {
    float3(0.0, -0.5, 0.0),
    float3(0.5, 0.5, 0.0),
    float3(-0.5, 0.5, 0.0)
};

struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT { float4 pos : SV_POSITION; };

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	VS_OUTPUT output;

	float3 hardcodedPos = positions[vertexID];
	output.pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0);
	// output.pos = float4(hardcodedPos.x, hardcodedPos.y, hardcodedPos.z, 1.0);

	return output;
}