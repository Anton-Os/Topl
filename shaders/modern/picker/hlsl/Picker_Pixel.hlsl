#include "Modern_Common.hlsl"

#include "Modern_Pixel.hlsl"

// Main: writes the id color untouched by lighting, fog or tonemapping

float4 main(PS_INPUT input) : SV_TARGET {
	return float4(actor_color.rgb, 1.0);
}
