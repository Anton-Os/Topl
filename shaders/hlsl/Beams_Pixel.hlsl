struct PS_INPUT {
	float4 pos : SV_POSITION;

	float4 ambient : COLOR0;
	float4 diffuse : COLOR1;
	float4 specular : COLOR2;
};

float4 main(PS_INPUT input) : SV_TARGET{
	return input.ambient;
}