cbuffer CONST_BLOCK : register(b0) {
	float4 offset;
	float4 rotation;
	// float4 color;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	float4 cameraPos;
	float4 lookPos;
	float4x4 projMatrix;

	float4 light1_pos;
	float3 light1_color; float light1_intensity;

	float4 light2_pos;
	float3 light2_color; float light2_intensity;

	// potentially room for 3rd light source
}

struct VS_INPUT {
	float4 pos : POSITION;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;

	float4 ambient : COLOR0;
	float4 diffuse : COLOR1;
	float4 specular : COLOR2;
};

float4x4 calcCameraMatrix(float3 cPos, float3 lPos){ // camera postion and target position
	float3 defaultUpVec = float3(0.0, 1.0, 0.0);

	float3 zAxis = normalize(lPos - cPos);
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

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) { // Only output is position
	VS_OUTPUT output;

	float4 finalPos = float4(0.0, 0.0, 0.0, 0.0); // Empty vector
	float4 finalTrans = float4(input.pos.x + offset.x, input.pos.y + offset.y, input.pos.z + offset.z, 1.0);

	// if (rotation.x != 0.0 || rotation.y != 0.0) { // Rotation operations
		float3x3 zRotMatrix = {
			cos(rotation.x), sin(rotation.x), 0,
			-1 * sin(rotation.x), cos(rotation.x), 0,
			0, 0, 1
		};
		
		float3x3 yRotMatrix = {
			cos(rotation.y), 0, sin(rotation.y),
			0, 1, 0,
			-1.0 * sin(rotation.y), 0, cos(rotation.y)
		};

		float3 rotCoords = mul(mul(zRotMatrix, yRotMatrix), float3(input.pos.x, input.pos.y, input.pos.z));
		finalPos = float4(rotCoords, 0.0);
	
	// }

	finalPos += finalTrans;

	float4x4 cameraMatrix = calcCameraMatrix(cameraPos, lookPos);
	output.pos = mul(finalPos, cameraMatrix); // no projection
	// output.pos = mul(mul(projMatrix, cameraMatrix), finalPos);
	output.ambient = float4(1.0f, 0.0, 0.0, 1.0f); // red
	output.diffuse = float4(0.0, 1.0f, 0.0, 1.0f); // green
	output.specular = float4(0.0, 0.0, 1.0f, 1.0f); // blue

	return output;
}