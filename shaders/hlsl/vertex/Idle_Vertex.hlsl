#define INCLUDE_EXTBLOCK

#include "../Common.hlsl"

// Values

float3 positions[3] = {
    float3(0.0, -0.5, 0.0),
    float3(0.5, 0.5, 0.0),
    float3(-0.5, 0.5, 0.0)
};

struct VS_OUTPUT { float4 pos : SV_POSITION; };

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID) {
	VS_OUTPUT output;

	float3 hardcodedPos = positions[vertexID];
	output.pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0);
	// output.pos = float4(hardcodedPos.x, hardcodedPos.y, hardcodedPos.z, 1.0);

	if(instanceID >= 1){
		if(instanceID % 5 == 1) output.pos = output.pos + float4(input.pos.x + 0.1, input.pos.y + 0.1, 0.0, 0.0);
		else if(instanceID % 5 == 2) output.pos = output.pos + float4(input.pos.x - 0.1, input.pos.y - 0.1, 0.0, 0.0);
		else if(instanceID % 5 == 3) output.pos = output.pos + float4(input.pos.x - 0.1, input.pos.y + 0.1, 0.0, 0.0);
		else if(instanceID % 5 == 4) output.pos = output.pos + float4(input.pos.x + 0.1, input.pos.y - 0.1, 0.0, 0.0);
	}

	return output;
}