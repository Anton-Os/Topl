// Values

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

	float3 ambient : COLOR0;
	float3 diffuse : COLOR1;
	float3 specular : COLOR2;
};

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(mode == 1){ // alternate mode
		// float3 light_color = input.ambient; // ambient test
		float3 light_color = input.diffuse; // diffuse test
		// float3 light_color = input.specular; // specular test
		return float4(light_color, 1.0f);
	} else if(mode == 2){ // depth mode
		float depth = sqrt(pow(input.pos1.x, 2) + pow(input.pos1.y, 2) + pow(input.pos1.z, 2)); // depth calculation
		return float4(depth, depth, depth, 1.0f);
		// return float4(pos, 1.0f);
	} else return float4(input.ambient + input.diffuse + input.specular, 1.0f); // light mode // default
}