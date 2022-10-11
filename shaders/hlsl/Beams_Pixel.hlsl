// Values

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 lampLight_pos: POSITION; // make sure lamp light position is correct

	float3 ambient : COLOR0;
	float3 diffuse : COLOR1;
	float3 specular : COLOR2;

	uint mode : MODE;
};

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(input.mode == 1){ // alternate mode
		float3 light_color = input.specular;
		return float4(light_color, 1.0f);
	} else if(input.mode == 2){ // depth mode
		float3 pos = input.lampLight_pos; // make lampLight target for calculation
		float depth = sqrt(pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2)); // depth calculation
		return float4(depth, depth, depth, 1.0f);
		// return float4(pos, 1.0f);
	} else return float4(input.ambient + input.diffuse, 1.0f); // light mode // default
}