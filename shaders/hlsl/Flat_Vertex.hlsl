// Values

cbuffer CONST_BLOCK : register(b0) {
	// uint renderID;
	float4 color;
	float3 offset;
	float2 rotation;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	uint mode;
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

// Functions

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

float4x4 calcCamMatrix(float3 cPos, float2 lPos){ // camera postion and relative look position
	float3x3 rm = calcRotMatrix(lPos);
	float4x4 rotMatrix = { rm[0][0], rm[0][1], rm[0][2], 0, rm[1][0], rm[1][1], rm[1][2], 0, rm[2][0], rm[2][1], rm[2][2], 0, 0, 0, 0, 1 };

	float4x4 camMatrix = {
		1.0, 0.0, 0.0, cPos.x,
		0.0, 1.0, 0.0, cPos.y,
		0.0, 0.0, 1.0, cPos.z,
		0.0, 0.0, 0.0, 1.0
	};

	return mul(camMatrix, rotMatrix);
}

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) { // Only output is position
	VS_OUTPUT output;

	float3 rotCoords = mul(calcRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(rotCoords.x, rotCoords.y, rotCoords.z, 1.0);

	float4x4 cameraMatrix = calcCamMatrix(cam_pos, float2(look_pos.x, look_pos.y));
	// output.pos = mul(projMatrix, output.pos + float4(offset, 0.0));
	output.pos = mul(projMatrix, mul(cameraMatrix, output.pos + float4(offset, 0.0)));

	if(mode == 1) { // alternate mode
		switch(vertexID % 4){
			case 0: output.flatColor = float4(1.0f, 1.0f, 0.0f, 0.8f); break; // substract blue
			case 1: output.flatColor = float4(1.0f, 0.0f, 1.0f, 0.8f); break; // substract green
			case 2: output.flatColor = float4(0.0f, 1.0f, 1.0f, 0.8f); break; // substract red
			case 3: output.flatColor = float4(1.0f, 1.0f, 1.0f, 0.8f); break; // white
		}
	}
	else output.flatColor = output.flatColor = color; // solid mode // default

	return output;
}