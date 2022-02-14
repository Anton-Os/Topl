struct PS_INPUT {
	float4 pos : SV_POSITION;

	float3 ambient : COLOR0;
	float3 diffuse : COLOR1;
	float3 specular : COLOR2;
};

float4 main(PS_INPUT input) : SV_TARGET{
	float3 light_color = input.ambient + input.diffuse;
	// if(light_color.r <= 0.05f && light_color.g <= 0.05f && light_color.b <= 0.05f) discard;
	return float4(light_color, 1.0f);
}