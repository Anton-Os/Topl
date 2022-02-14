cbuffer CONST_BLOCK : register(b0) {
	float4 offset;
	float4 rotation;
	float4 color;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;
}

struct VS_INPUT {
	float4 pos : POSITION;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float4 flatColor : COLOR0;
};

float3x3 calcRotMatrix(float2 rotCoords){
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

	return mul(zRotMatrix, yRotMatrix);
}

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

	float4 final_pos = float4(0.0, 0.0, 0.0, 0.0); // Empty vector
	float4 final_trans = float4(input.pos.x + offset.x, input.pos.y + offset.y, input.pos.z + offset.z, 1.0);

	float3 rotCoords = mul(calcRotMatrix(float2(rotation.x, rotation.y)), float3(input.pos.x, input.pos.y, input.pos.z));
	final_pos = float4(rotCoords, 0.0) + final_trans; // rotation and translation

	float4x4 cameraMatrix = calcCameraMatrix(cam_pos, look_pos);
	output.pos = mul(final_pos, cameraMatrix); // no projection
	// output.pos = mul(mul(projMatrix, cameraMatrix), final_pos);
	output.flatColor = color;

	return output;
}