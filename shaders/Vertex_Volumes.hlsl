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
	float4 flatColor : COLOR0;
};

float4x4 calcCameraMatrix(float3 cPos, float3 lPos){ // camera postion and target position
	float3 defaultUpVec = float3(0.0, 1.0, 0.0);

	float3 zAxis = normalize(lPos - cPos); // values flipped because of coordinate system
	float3 xAxis = normalize(cross(defaultUpVec, zAxis));
	float3 yAxis = cross(zAxis, xAxis);

	float4x4 camMatrix = {
		xAxis.x, yAxis.x, zAxis.x, 0.0,
		xAxis.y, yAxis.y, zAxis.y, 0.0,
		xAxis.z, yAxis.z, zAxis.z, 0.0,
		-1.0 * dot(xAxis, cPos), -1.0 * dot(yAxis, cPos), -1.0 * dot(zAxis, cPos), 1.0
	};

	return camMatrix;
}

VS_OUTPUT main(VS_INPUT input) { // Only output is position
	VS_OUTPUT output;

	float4 finalPos = float4(0.0, 0.0, 0.0, 0.0); // Empty vector
	float4 finalTranslation = float4(input.pos.x + offset.x, input.pos.y + offset.y, input.pos.z + offset.z, 1.0);

	// if (rotation.x != 0.0 || rotation.y != 0.0) { // Rotation operations
		float2x2 zRotMatrix = {
			cos(rotation.x), sin(rotation.x),
			-1 * sin(rotation.x), cos(rotation.x)
		};
		float2 zRotCoords = mul(zRotMatrix, float2(input.pos.x, input.pos.y)); // Rotation coordinates over z axis!
		
		float3x3 yRotMatrix = {
			cos(rotation.y), 0, -1 * sin(rotation.y), // cos(rotation.y), 0, -1 * sin(rotation.y),
			0, 1, 0,
			sin(rotation.y), 0, cos(rotation.y) // sin(rotation.y), 0, cos(rotation.y)
		};
		float3 finalRotCoords = mul(yRotMatrix, float3(zRotCoords.x, zRotCoords.y, input.pos.z));

		finalPos = float4(finalRotCoords, 0.0);
	
	// }

	finalPos += finalTranslation;

	float4x4 cameraMatrix = calcCameraMatrix(cameraPos, lookPos);
	// output.pos = mul(mul(projMatrix, cameraMatrix), finalPos);
	output.pos = mul(cameraMatrix, finalPos); // no projection
	output.flatColor = float4(0.4f, 0.8f, 0.2f, 1.0f);

	return output;
}