#define INCLUDE_MESHBLOCK
#define IGNORE_INPUTS

#define PATTERN_SIZE 0.025

#include "Common.hlsl"

#include "_Pixel.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
    float3 offset;
    float3 rotation;
    float3 scale;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

    double timeFrame;
	double timeElapse;
	float2 cursorPos;
}

struct PS_INPUT { 
	float4 pos : SV_POSITION; 
	float3 vertex_pos: POSITION1;
	float3 vertex_color : COLOR;
	float3 normal: NORMAL;
	float3 texcoord: TEXCOORD;
	float3 tangent: TANGENT;
};

#include "Pattern.hlsl"

// Main

float4 pattern_effect(float3 coords, uint m, double a){
	float3 origin = float3(cursorPos, distance(cursorPos, coords));
	if(coords.x == 0 && coords.y == 0 && coords.z == 0) return float4(0, 0, 0, 0.0); // returns transparent if coords are zeroed out by Custom_Pattern
	else if(m % 10 == 1) return float4(pattern1(coords, origin), a);
	else if(m % 10 == 2) return float4(pattern2(coords, origin), a);
	else if(m % 10 == 3) return float4(pattern3(coords, origin), a);
	else if(m % 10 == 4) return float4(pattern4(coords, origin), a);
	else if(m % 10 == 5) return float4(pattern5(coords, origin), a);
	else if(m % 10 == 6) return float4(pattern6(coords, origin), a);
	else if(m % 10 == 7) return float4(pattern7(coords, origin), a);
	else if(m % 10 == 8) return float4(pattern8(coords, origin), a);
	else if(m % 10 == 9) return float4(pattern9(coords, origin), a);
	else return float4(coords, a);
}

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET{
	float4 outColor;
	float3 coords = input.vertex_pos; // float3(input.pos.x, input.pos.y, input.pos.z);

	// return float4(input.texcoord * input.vertex_color, 1.0); 
	if(abs(mode) % 10 == 1) coords = float3(input.pos.x, input.pos.y, input.pos.z);
	else if(abs(mode) % 10 == 2) coords = input.vertex_color;
	else if(abs(mode) % 10 == 3) coords = input.normal;
	else if(abs(mode) % 10 == 4) coords = input.tangent;
	else if(abs(mode) % 10 == 5) coords = input.texcoord;
	else if(abs(mode) % 10 == 6) coords = getRandColor(primID);
	else if(abs(mode) % 10 == 7) coords = (input.vertex_pos * input.texcoord) + (input.normal / input.tangent);
	else if(abs(mode) % 10 == 8) coords = float3(pow(abs(input.vertex_pos.x), abs(input.tangent.x)), pow(abs(input.texcoord.y), abs(input.tangent.y)), pow(abs(input.normal.z), abs(input.tangent.z)));
	else if(abs(mode) % 10 == 9) coords = float3(dot(input.normal, input.tangent), dot(input.texcoord, input.vertex_color), dot(getRandColor(primID), input.vertex_pos));

	outColor = pattern_effect(coords, abs(mode / 10), 1.0);
	if(mode > 0) outColor = pattern_effect(-coords, abs(mode / 10), 1.0);

	if(abs(mode / 100) >= 1)
		for(uint i = 1; i <= abs(mode / 100) && i < 10; i++)
			outColor = pattern_effect(coords * outColor.rgb, abs(mode / 10), 1.0);
#ifdef INCLUDE_TEXTURES
	outColor *= modalTex(abs(mode / 1000), input.texcoord);
#endif
	return outColor;
}