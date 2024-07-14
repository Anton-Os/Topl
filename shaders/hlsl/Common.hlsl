#ifdef INCLUDE_BLOCK
cbuffer CONST_BLOCK : register(b0) {
	float3 offset;
	float3 rotation;
	float3 scale;
}
#endif

#ifdef INCLUDE_SCENEBLOCK
cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;
}
#endif

/* cbuffer CONST_ENV_BLOCK : register(b2) {
	float4 controlPoints[64];
	float4 nearestVertex[1024];
} */

#ifndef IGNORE_INPUTS
struct VS_INPUT {
	float4 pos : POSITION;
	float3 texcoord : TEXCOORD;
};
#endif

// Functions

uint4 getModes(uint mode){
	return uint4( mode % 10, (mode - (mode % 10)) / 10, (mode - (mode % 100)) / 100, (mode - (mode % 1000)) / 1000);
}

float4 getRandColor(float4 seedColor){
	float4 randColor = seedColor * float4(999.82, 111.15, 454.55, 1.0);

	while(randColor.x > 1.0) randColor.r -= 0.5; // pow(randColor.x, 0.1);
	while(randColor.y > 1.0) randColor.g -= 0.5; // pow(randColor.y, 0.1);
	while(randColor.z > 1.0) randColor.b -= 0.5; // pow(randColor.z, 0.1);

	return randColor;
}

float4 getUniqueColor(int id){
	float attenuation = floor(id / 6.0) * 0.025;

	if(id % 6 == 0) return float4(1.0 - attenuation, 0.0, 0.0, 1.0); // red
	else if (id % 6 == 1) return float4(0.0, 1.0 - attenuation, 0.0, 1.0); // green
	else if (id % 6 == 2) return float4(0.0, 0.0, 1.0 - attenuation, 1.0); // blue
	else if (id % 6 == 3) return float4(1.0 - attenuation, 1.0 - attenuation, 0.0, 1.0); // yellow
	else if (id % 6 == 4) return float4(0.0, 1.0 - attenuation, 1.0 - attenuation, 1.0); // cyan
	else if (id % 6 == 5) return float4(1.0 - attenuation, 0.0, 1.0 - attenuation, 1.0); // magenta
	else return float4(1.0 - attenuation, 1.0 - attenuation, 1.0 - attenuation, 1.0); // white
}

float3x3 getRotMatrix(float3 angles) {
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

float4x4 getCamMatrix(float4 cPos, float3 angles) { // camera postion and relative look position
	float4x4 camMatrix = {
		cos(angles.z) * cos(angles.x), -sin(angles.x), sin(angles.z), -cPos.x,
		sin(angles.x), cos(angles.x) * cos(angles.y), sin(angles.y), -cPos.y,
		-1.0 * sin(angles.z), -sin(angles.y), cos(angles.y) * cos(angles.z), -cPos.z,
		0, 0, 0, 1
	};

	return camMatrix;
}

// TODO: Include other helper functions