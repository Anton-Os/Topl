// Values

struct PS_INPUT { 
	float4 pos : SV_POSITION; 
	float4 color : COLOR0;
};

// Main

float4 main(PS_INPUT input) : SV_TARGET{ return input.color; }