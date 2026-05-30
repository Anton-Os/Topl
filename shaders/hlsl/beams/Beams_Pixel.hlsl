#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#define BEAMS_FULL 0
#define BEAMS_AMBIENT 1
#define BEAMS_DIFFUSE 2
#define BEAMS_SPECULAR 3
#define BEAMS_HIGHLIGHT 4
#define BEAMS_SPOT 5
#define BEAMS_DEPTH 6
#define BEAMS_DISTANCE 7
#define BEAMS_TRAJECTORY 8
#define BEAMS_TRIAL 9

#include "Common.hlsl"

#include "Pixel.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	float3 skyLight[2]; // position and value
	float3 flashLight[2]; // position and value
	float3 lampLight[2]; // position and value
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 vertex_pos : POSITION;
	float3 normal : NORMAL;
	float3 texcoord : TEXCOORD;
};

uint getLightCount(uint mode){ // used for determining how many lights to combine
    uint count = 1;
    if(mode >= 3) count = 2;
    if(mode >= 6) count = 3;
    return count;
}

#ifdef INCLUDE_TEXTURES
#include "beams/Textured.hlsl"
#else
#include "beams/Flat.hlsl"
#endif

// Main

float4 main(PS_INPUT input) : SV_TARGET{
    uint4 modes = getModes(mode);
	uint intensity = modes[3] + 1;

	float3 target;
	if(mode >= 0) target = input.normal; 
	else target = input.vertex_pos;
	
    float3 lights[3][2];
    if(modes[1] % 3 == 1){ lights[0] = flashLight; lights[1] = lampLight; lights[2] = skyLight; }
    else if(modes[1] % 3 == 2){ lights[0] = lampLight; lights[1] = skyLight; lights[2] = flashLight; }
    else{ lights[0] = skyLight; lights[1] = flashLight; lights[2] = lampLight; }

	float cam_dist = distance(target, float3(cam_pos.x, cam_pos.y, cam_pos.z));
#ifdef INCLUDE_TEXTURES
	float4 texTarget = modalTex(modes[3], input.texcoord);
	float3 ambient = (texTarget.rgb + getAmbient_sampled(lights, intensity)) * 0.5;
	float3 diffuse = getDiffuse_sampled(lights, target, intensity);
	float3 specular = getSpecular_sampled(lights, target, intensity);
#else
	float3 ambient = getAmbient_flat(lights, intensity);
	float3 diffuse = getDiffuse_flat(lights, target, intensity);
	float3 specular = getSpecular_flat(lights, target, intensity);
#endif

	if(modes[0] == BEAMS_AMBIENT) return float4(ambient, 1.0f);
	else if(modes[0] == BEAMS_DIFFUSE) return float4(diffuse, 1.0f);
	else if(modes[0] == BEAMS_SPECULAR) return float4(specular, 1.0f);
	else if(modes[0] == BEAMS_HIGHLIGHT) return float4(ambient + (lights[0][1] * dot(normalize(float3(cam_pos.x, cam_pos.y, cam_pos.z)), normalize(target))), 1.0);
	else if(modes[0] == BEAMS_SPOT) return float4(ambient.r + pow(specular.r, 1.0 / diffuse.r), ambient.g + pow(specular.g, 1.0 / diffuse.g), ambient.b + pow(specular.b, 1.0 / diffuse.b), 1.0);
	else if(modes[0] == BEAMS_DEPTH) return float4(float3(cam_dist, cam_dist, cam_dist) * lights[0][1], 1.0f);
	else if(modes[0] == BEAMS_DISTANCE) return float4(length(lights[0][1] - target), length(lights[1][1] - target), length(lights[2][1] - target), 1.0f);
	// else if(modes[0] == BEAMS_DISTANCE) return float4(ambient + (cam_dist * diffuse) + specular, 1.0f);
	else if(modes[0] == BEAMS_TRAJECTORY) return float4(ambient + (distance(target, lights[0][0]) * diffuse) + (specular * cross(target, lights[0][0])), 1.0f);
	// else if(modes[0] == BEAMS_TRAJECTORY) return float4(lights[0][1] * normalize(cross(lights[0][0] - float3(cam_pos.x, cam_pos.y, cam_pos.z), target)), 1.0);
	else if(modes[0] == BEAMS_TRIAL) return float4(ambient + float3(cos(1.0 / (diffuse.r * specular.r)), sin(1.0 / (specular.g * diffuse.g)), tan(diffuse.b - specular.b)), 1.0);
	else return float4(ambient + diffuse + specular, 1.0);
}