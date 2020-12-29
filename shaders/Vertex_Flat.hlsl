cbuffer CONST_BLOCK : register(b0) {
	float4 offset;
	float4 rotation;
	uint flatColor; // shader object will accept these as inputs
}

struct VS_INPUT {
	float4 pos : POSITION;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	uint flatColor : COLOR0;
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

	output.pos += offset;
	output.flatColor = 0xFF884422; // TODO: Make this equal to flatColor input

	return output;
}