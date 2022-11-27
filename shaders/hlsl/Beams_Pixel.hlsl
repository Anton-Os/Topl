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

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 pos1 : POSITION;
};

// Functions

float calcSpec(float3 camera, float3 vertex) { // Custom Function
	float intensity = dot(normalize(camera), normalize(vertex));
	return max(pow(intensity * 5, 5), 0);
	// return max(pow(intensity, 3), 0);
}

float calcDiffuse(float3 light, float3 vertex) {
	float intensity = dot(normalize(light), normalize(vertex));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float3 ambient = skyLight_value * 0.2;
	float3 diffuse = skyLight_value * calcDiffuse(skyLight_pos, input.pos1 - offset) * 0.5;
	float3 specular = skyLight_value * calcSpec(cam_pos, input.pos1);

	if(mode == 1){ // alternate mode
		// float3 light_color = ambient; // ambient test
		// float3 light_color = diffuse; // diffuse test
		float3 light_color = specular; // specular test
		return float4(light_color, 1.0f);
	} else if(mode == 2){ // depth mode
		float depth = sqrt(pow(input.pos1.x, 2) + pow(input.pos1.y, 2) + pow(input.pos1.z, 2)); // depth calculation
		return float4(depth, depth, depth, 1.0f);
		// return float4(pos, 1.0f);
	}
	else return float4(ambient + diffuse + specular, 1.0); // light mode // default
}