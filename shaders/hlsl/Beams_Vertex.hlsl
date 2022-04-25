// #include "Utility.hlsl"

cbuffer CONST_BLOCK : register(b0) {
	float3 offset;
	float2 rotation;
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
	float3 lampLight_pos : POSITION;

	float3 ambient : COLOR0;
	float3 diffuse : COLOR1;
	float3 specular : COLOR2;

	uint mode : MODE;
};

float sharpen(float intensity, uint curve){
	const float scale = 5.0f;

	if(intensity <= 0.0) return intensity;
	else return pow(intensity, curve) * pow(scale, curve);
}

float calcSpecIntensity(float3 light, float3 target, float3 camera){
	float lightDP = dot(normalize(light), target); // light dot product
	float camDP = dot(normalize(camera), target); // camera dot product

	// return dot(camera, reflectVec);
	// return 0.5 * dot(float3(0.0f, 0.0f, -1.0f), normalize(reflectVec));
	return sharpen(lightDP, 2);
}

float calcDiffuseIntensity(float3 light, float3 target){
	float intensity = dot(normalize(light), normalize(target));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light)); // length * length is equal to length^2
	// return intensity;
	return intensity * attenuation;
}


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

	// Value Shadings
	float3 vertex_pos = float3(input.pos.x, input.pos.y, input.pos.z);
	float4 transCoords = float4(input.pos.x + offset.x, input.pos.y + offset.y, input.pos.z + offset.z, 1.0);
	float3 rotCoords = mul(calcRotMatrix(float2(rotation.x, rotation.y)), vertex_pos);	
	float4 final_pos = float4(rotCoords, 0.0) + transCoords; // rotation and translation

	float4x4 cameraMatrix = calcCameraMatrix(cam_pos, look_pos);
	output.pos = mul(final_pos, cameraMatrix); // no projection
	// output.pos = mul(mul(transpose(projMatrix), cameraMatrix), final_pos);
	output.lampLight_pos = lampLight_pos;

	// Light Source Shadings
	const float ambient_intensity = 0.1f; // ambient light intensity
	output.ambient = ambient_intensity * skyLight_value; // only sky light affects ambient property
	const float skyLight_intensity = calcDiffuseIntensity(skyLight_pos, vertex_pos);
	const float flashLight_intensity = calcDiffuseIntensity(flashLight_pos, vertex_pos);
	output.diffuse = (skyLight_intensity * skyLight_value) + (flashLight_intensity * flashLight_value);
	const float specular_intensity = calcSpecIntensity(flashLight_pos, vertex_pos, cam_pos);
	output.specular = specular_intensity * flashLight_value; // only flash light affects specular

	output.mode = mode;

	return output;
}