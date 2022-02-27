struct PS_INPUT {
	float4 pos : SV_POSITION;

	float3 ambient : COLOR0;
	float3 diffuse : COLOR1;
	float3 specular : COLOR2;
};

float4 main(PS_INPUT input) : SV_TARGET{
	// float3 light_color = input.ambient + input.diffuse + input.specular;
	float3 light_color = input.specular;
	return float4(light_color, 1.0f);
}