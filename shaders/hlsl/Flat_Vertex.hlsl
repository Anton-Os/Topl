#define LOOK_RADIUS 2

// Values

cbuffer CONST_BLOCK : register(b0) {
	float4 color;
	float3 offset;
	float3 rotation;
	float3 scale;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;
}

struct VS_INPUT { float4 pos : POSITION; };

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	// float4 flatColor : COLOR0;
	uint vertexNo : VERTEXID;
	float4 pos1 : POSITION0;
};

// Functions

float3x3 calcRotMatrix(float3 angles){
	float3x3 zRotMatrix = { // Roll
		cos(angles.z), -sin(angles.z), 0,
		sin(angles.z), cos(angles.z), 0,
		0, 0, 1
	};

	float3x3 xRotMatrix = { // Pitch
		1, 0, 0,
		0, cos(angles.y), sin(angles.y),
		0, -sin(angles.y), cos(angles.y)
	};
	
	float3x3 yRotMatrix = { // Yaw
		cos(angles.x), 0, sin(angles.x),
		0, 1, 0,
		-sin(angles.x), 0, cos(angles.x)
	};

	return mul(mul(xRotMatrix, yRotMatrix), zRotMatrix);
}

float4x4 calcCamMatrix(float3 cPos, float3 lPos){ // lookAt matrix in progress
	float4x4 camMatrix = {
		1, 0, 0, -cPos.x,
		0, 1, 0, -cPos.y,
		0, 0, 1, cPos.z - LOOK_RADIUS,
		0, 0, 0, 1
	};

	float3x3 rotMatrix = calcRotMatrix(lPos);

	float4x4 camRotMatrix = {
		rotMatrix[0][0], rotMatrix[0][1], rotMatrix[0][2], 0,
		rotMatrix[1][0], rotMatrix[1][1], rotMatrix[1][2], 0,
		rotMatrix[2][0], rotMatrix[2][1], rotMatrix[2][2], 0,
		0, 0, 0, 1
	};

	// return camMatrix;
	return mul(camRotMatrix, camMatrix);
}

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) { // Only output is position
	VS_OUTPUT output;

	float3 angles = mul(calcRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x + offset.x, angles.y + offset.y, angles.z + (-offset.z + 0.5) * 2, 1.0);
	output.pos *= float4(scale.x, scale.y, scale.z, 1.0);

	float4x4 cameraMatrix = calcCamMatrix(cam_pos, look_pos);
	output.pos = mul(transpose(projMatrix), mul(cameraMatrix, output.pos));
	output.pos -= float4(0, 0, LOOK_RADIUS, 0); // move back to offset camera location

	output.vertexNo = vertexID;
	output.pos1 = output.pos;

	return output;
}