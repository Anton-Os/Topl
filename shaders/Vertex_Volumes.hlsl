cbuffer CONST_BLOCK : register(b0) {
	float4 offset; // padding in type
	float4 rotation; // padding in type
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	float4 cameraPos; // padding in type
	float4 lookPos; // padding in type
	float4x4 projMatrix;
}

struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

float4x4 calcCameraMatrix(float3 cPos, float3 lPos){ // camera postion and target position
	float3 defaultUpVec = (0.0, 1.0, 0.0);

	float3 zAxis = normalize(cPos - lPos);
	float3 xAxis = normalize(cross(defaultUpVec, zAxis));
	float3 yAxis = cross(zAxis, xAxis);

	float4x4 lookAtMatrix = {
		xAxis.x, xAxis.y, xAxis.z, 0.0,
		yAxis.x, yAxis.y, yAxis.z, 0.0,
		zAxis.x, zAxis.y, zAxis.z, 0.0,
		0.0, 0.0, 0.0, 1.0
	};

	float4x4 eyeMatrix = {
		1.0, 0.0, 0.0, -1.0 * cPos.x,
		0.0, 1.0, 0.0, -1.0 * cPos.y,
		0.0, 0.0, 1.0, -1.0 * cPos.z,
		0.0, 0.0, 0.0, 1.0,
	};

	return mul(eyeMatrix, lookAtMatrix);
}

VS_OUTPUT main(VS_INPUT input) { // Only output is position
	VS_OUTPUT output;

	// output.pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0); // Initial assignment
	float4 finalPos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0);

	if (rotation.x != 0.0) { // Rotation operations
		float2x2 zRotMatrix = {
			cos(rotation.x), sin(rotation.x),
			-1 * sin(rotation.x), cos(rotation.x)
		};

		// float3x3 yRotMatrix = {} // TODO: Implement this

		float2 rotCoords = mul(zRotMatrix, float2(input.pos.x, input.pos.y));
		finalPos.x = rotCoords.x; finalPos.y = rotCoords.y;
		finalPos.x += offset.x; finalPos.y += offset.y; finalPos.z += offset.z;
	}

	output.texcoord = float2(input.texcoord[0], input.texcoord[1]);
	output.pos = mul(projMatrix, finalPos); // TODO: Add camera matrix

	return output;
}