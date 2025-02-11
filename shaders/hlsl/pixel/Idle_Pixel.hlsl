#define INCLUDE_EXTBLOCK

#include "../Common.hlsl"

// Values

struct PS_INPUT { float4 pos : SV_POSITION; };

// Main

float4 main(PS_INPUT input) : SV_TARGET{
    if(vertCount == 4) return float4(0.0, 1.0, 1.0, 1.0); // tesselated draw
    else if(vertCount == 0) return float4(1.0, 0.0, 1.0, 1.0); // default draw 
    return float4(1.0, 1.0, 0.0, 1.0); // primitive draw
}