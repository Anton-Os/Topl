struct VS_INPUT {
	float4 pos : POSITION;
	float3 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 vert_color : COLOR;
	float3 tangent : TANGENT;
};

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

float4x4 getCamMatrix(float4 cPos, float3 angles) {
	float3x3 rotMatrix = getRotMatrix(angles);

	float4x4 camMatrix = {
		rotMatrix[0][0], rotMatrix[0][1], rotMatrix[0][2], -cPos.x,
		rotMatrix[1][0], rotMatrix[1][1], rotMatrix[1][2], -cPos.y,
		rotMatrix[2][0], rotMatrix[2][1], rotMatrix[2][2], -cPos.z,
		0, 0, 0, 1
	};

	return camMatrix;
}

float4x4 getLookAtMatrix(float3 cPos, float3 aRot){
	float3 lPos = float3(sin(-aRot.y), sin(aRot.x), -cos(aRot.x + aRot.y));
	float3 uPos = float3(0.0, sin(aRot.z), cos(aRot.z)); // computed up position

	float3 forward = normalize(lPos);
	float3 right = normalize(cross(forward, uPos));
	float3 up = normalize(cross(right, uPos));
	
	float4x4 lookAtMatrix = {
		right.x, right.y, right.z, -cPos.x,
		up.x, up.y, up.z, -cPos.y,
		-forward.x, -forward.y, -forward.z, -cPos.z,
		0, 0, 0, 1
	};

	float4x4 camMatrix = {
		1, 0, 0, -cPos.x,
		0, 1, 0, -cPos.y,
		0, 0, 1, -cPos.z,
		0, 0, 0, 1
	};

	if(aRot.x != 0.0 || aRot.y != 0.0 || aRot.z != 0.0) return mul(lookAtMatrix, camMatrix);
	else return camMatrix;
	// return getCamMatrix(float4(cPos, 1), lPos); // lookMatrix * placeMatrix;
}

float getLineDistance(float2 coord, float2 p1, float2 p2){
	return abs(((p2.y - p1.y) * coord.x) - ((p2.x - p1.x) * coord.y) + (p2.x * p1.y) - (p2.y * p1.x)) / sqrt(pow(p2.y - p1.y, 2.0) + pow(p2.x - p1.x, 2.0));
}

float3 getCoordDistances(float2 coord, float2 p1, float2 p2){
	return float3(
		sqrt(pow(p2.x - p1.x, 2.0) + pow(p2.y - p1.y, 2.0)), // distance between points 1 and 2
		sqrt(pow(coord.x - p1.x, 2.0) + pow(coord.y - p1.y, 2.0)), // distance between coordinate and point 1 
		sqrt(pow(coord.x - p2.x, 2.0) + pow(coord.y - p2.y, 2.0)) // distance between coordinate and point 2
	);
}

float4 getVertex(float3 input, float3 translation, float3 degrees, float4 size){
	float3 rotation = mul(getRotMatrix(degrees), input);
	float4 pos = float4(rotation, 1.0f) * size;
	pos += float4(translation, 0.0f);

	return pos;
}