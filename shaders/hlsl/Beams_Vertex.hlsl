// Values

cbuffer CONST_BLOCK : register(b0) {
	// uint renderID;
	float3 offset;
	float3 rotation;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	uint mode;
	float4 cam_pos;
	float4 look_pos;
	// float4x4 projMatrix;

	float3 skyLight_pos; float3 skyLight_value;
	float3 flashLight_pos; float3 flashLight_value;
	float3 lampLight_pos; float3 lampLight_value;
}

struct VS_INPUT {
	float4 pos : POSITION;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 pos1 : POSITION;

	float3 ambient : COLOR0;
	float3 diffuse : COLOR1;
	float3 specular : COLOR2;
};

// Functions

float sharpen(float intensity, uint curve){
	const float scale = 5.0f;

	if(intensity <= 0.0) return intensity;
	else return pow(intensity, curve) * pow(scale, curve);
}

float calcSpec(float3 light, float3 target, float3 camera){
	float lightDP = dot(normalize(light), target); // light dot product
	float camDP = dot(normalize(camera), target); // camera dot product

	// return dot(camera, reflectVec);
	// return 0.5 * dot(float3(0.0f, 0.0f, -1.0f), normalize(reflectVec));
	return sharpen(lightDP, 2);
}

float calcDiffuse(float3 light, float3 target){
	float intensity = dot(normalize(light), normalize(target));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

float3x3 calcRotMatrix(float3 angles) {
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

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) { // Only output is position
	VS_OUTPUT output;

	float3 angles = mul(calcRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x, angles.y, angles.z, 1.0);

	float4x4 cameraMatrix = calcCameraMatrix(cam_pos, look_pos); // TODO: include camera matrix with projection
	output.pos += float4(offset, 0.0f); // output.pos += mul(projMatrix, offset);
	output.pos1 = float3(output.pos.x, output.pos.y, output.pos.z);

	// ambient shading
	const float ambient_intensity = 0.25f; // ambient light intensity
	output.ambient = ambient_intensity * skyLight_value;
	// diffuse shading
	const float skyLight_diffuse = calcDiffuse(skyLight_pos, angles);
	output.diffuse = (skyLight_diffuse * skyLight_value); // + (flashLight_diffuse * flashLight_value);
	// specular shading
	const float specular_intensity = calcSpec(skyLight_pos, angles, cam_pos);
	output.specular = specular_intensity * skyLight_value;

	return output;
}