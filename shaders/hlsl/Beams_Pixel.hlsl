// Values

struct PS_INPUT {
	uint mode : MODE;
	float4 pos : SV_POSITION;
	float3 pos1 : POSITION;

	float3 ambient : COLOR0;
	float3 diffuse : COLOR1;
	float3 specular : COLOR2;
};

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(input.mode == 1){ // alternate mode
		float3 light_color = input.specular;
		return float4(light_color, 1.0f);
	} else if(input.mode == 2){ // depth mode
		float depth = sqrt(pow(input.pos1.x, 2) + pow(input.pos1.y, 2) + pow(input.pos1.z, 2)); // depth calculation
		return float4(depth, depth, depth, 1.0f);
		// return float4(pos, 1.0f);
	} else return float4(input.ambient + input.diffuse, 1.0f); // light mode // default
}