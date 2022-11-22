// Values

cbuffer CONST_BLOCK : register(b0) {
	// uint renderID;
	float4 color;
	float3 offset;
	float3 rotation;
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

float3x3 calcRotMatrix(float3 angles){
	float3x3 zRotMatrix = { // Roll
		cos(angles.x), sin(angles.x), 0,
		-1 * sin(angles.x), cos(angles.x), 0,
		0, 0, 1
	};

	float3x3 xRotMatrix = { // Pitch
		1, 0, 0,
		0, cos(angles.y), sin(angles.y),
		0, -sin(angles.y), cos(angles.y)
	};
	
	float3x3 yRotMatrix = { // Yaw
		cos(angles.z), 0, sin(angles.z),
		0, 1, 0,
		-1.0 * sin(angles.z), 0, cos(angles.z)
	};

	return mul(mul(zRotMatrix, yRotMatrix), xRotMatrix);
}

float4x4 calcCamMatrix(float3 cPos, float3 lPos){ // camera postion and relative look position
	float3 forward = normalize(lPos - cPos);
	/* float3 up = float3(0, 1, 0);
	float3 right = cross(forward, up);
	
	float4x4 camMatrix = {
		right.x, up.x, forward.x, cPos.x,
		right.y, up.y, forward.y, cPos.y,
		right.z, up.z, forward.z, cPos.z,
		0.0, 0.0, 0.0, 1.0
	}; */

	float4x4 camMatrix = {
		1, 0, 0, -cPos.x,
		0, 1, 0, -cPos.y,
		0, 0, 1, -cPos.z,
		0, 0, 0, 1
	};

	return camMatrix; // TODO: Multiply by calculated rotation
}

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) { // Only output is position
	VS_OUTPUT output;

	float3 angles = mul(calcRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x, angles.y, angles.z, 1.0);

	float4x4 cameraMatrix = calcCamMatrix(cam_pos, look_pos);
	output.pos = mul(transpose(projMatrix), mul(cameraMatrix, output.pos + float4(offset, 0.0)));

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