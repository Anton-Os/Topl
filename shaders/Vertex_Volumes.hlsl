cbuffer CONST_BLOCK : register(b0) {
	float4 offset;
	float4 rotation;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	float4 cameraPos; // padding in type
	float4 lookPos; // padding in type
	float4x4 projMatrix;
}

struct VS_INPUT {
	float4 pos : POSITION;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	uint flatColor : COLOR0;
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

	float4 finalPos = float4(0.0, 0.0, 0.0, 0.0); // Empty vector
	float4 finalTranslation = float4(input.pos.x + offset.x, input.pos.y + offset.y, input.pos.z + offset.z, 1.0);

	if (rotation.x != 0.0 || rotation.y != 0.0) { // Rotation operations

		float2x2 zRotMatrix = {
			cos(rotation.x), sin(rotation.x),
			-1 * sin(rotation.x), cos(rotation.x)
		};
		float2 zRotCoords = mul(zRotMatrix, float2(input.pos.x, input.pos.y)); // Rotation coordinates over z axis!
		
		
		float3x3 yRotMatrix = {
			cos(rotation.y), 0, -1 * sin(rotation.y),
			0, 1, 0,
			sin(rotation.y), 0, cos(rotation.y)
		};
		float3 yRotCoords = mul(yRotMatrix, float3(input.pos.x, input.pos.y, input.pos.z)); // Rotation coordinates over y axis!


		finalPos.x = zRotCoords.x + yRotCoords.x;
		finalPos.y = zRotCoords.y + yRotCoords.y;
		finalPos.z = yRotCoords.z;
	}

	finalPos += finalTranslation;

	output.pos = mul(projMatrix, finalPos); // TODO: Multiply by computer cameraMatrix later
	output.flatColor = 0xFF884422;

	return output;
}