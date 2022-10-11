// Values

cbuffer CONST_BLOCK : register(b0) {
	// uint actorID
	float3 offset;
	float2 rotation;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	float4 look_pos;
	float4 cam_pos;
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

// Functions

float3x3 calcRotMatrix(float2 rotCoords) {
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

// Main

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	float3 rotCoords = mul(calcRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(rotCoords.x, rotCoords.y, input.pos.z, 1.0);

	output.pos += mul(projMatrix, offset);
	output.texcoord = float2(input.texcoord[0], input.texcoord[1]);

	return output;
}