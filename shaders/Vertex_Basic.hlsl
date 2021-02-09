cbuffer CONST_BLOCK : register(b0) {
	float4 offset;
	float4 rotation;
}

struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) { // Only output is position
	VS_OUTPUT output;

	output.pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0); // Initial assignment

	if (rotation.x != 0.0) { // Rotation operations
		float2x2 rotMatrix = {
			cos(rotation.x), sin(rotation.x),
			-1 * sin(rotation.x), cos(rotation.x)
		};

		float2 rotCoords = mul(rotMatrix, float2(input.pos.x, input.pos.y));
		output.pos.x = rotCoords.x;
		output.pos.y = rotCoords.y;
	}

	output.texcoord = float2(input.texcoord[0], input.texcoord[1]);
	output.pos += offset;

	return output;
}