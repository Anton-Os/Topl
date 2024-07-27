#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

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
};

// Functions

float calcSpec(float4 camera, float3 vertex) { // Custom Function
	float intensity = dot(normalize(float3(camera.x, camera.y, camera.z)), normalize(vertex));
	return max(pow(intensity * 3, 5), 0);
}

float calcDiffuse(float3 light, float3 vertex) {
	float intensity = dot(normalize(light), normalize(vertex));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
    uint4 modes = getModes(mode);
	
    float3 lights[3][2];
    if(modes[1] % 3 == 1){ lights[0] = flashLight; lights[1] = lampLight; lights[2] = skyLight; }
    else if(modes[1] % 3 == 2){ lights[0] = lampLight; lights[1] = skyLight; lights[2] = flashLight; }
    else{ lights[0] = skyLight; lights[1] = flashLight; lights[2] = lampLight; }

	float3 ambient = lights[0][1] * 0.3;
	float3 diffuse = lights[0][1] * calcDiffuse(lights[0][0], input.vertex_pos - offset) * 0.5;
	float3 specular = lights[0][1] * calcSpec(cam_pos, input.vertex_pos);

    if(modes[1] >= 3){ // combining lights
        uint count = 2;
        if(modes[1] >= 6) count = 3;
        for(uint l = 1; l < count; l++){
            ambient += (lights[l][1] * 0.3) * (1.0 / count);
            diffuse += (lights[l][1] * calcDiffuse(lights[l][0], input.vertex_pos - offset) * 0.5) * (1.0 / count);
            specular += (lights[l][1] * calcSpec(cam_pos, input.vertex_pos)) * (1.0 / count);
        }
    }

	if(mode < 0){
		float3 distVec = lights[0][0] - input.vertex_pos - offset;
		float dist = sqrt(pow(distVec.x, 2) + pow(distVec.y, 2) + pow(distVec.z, 2));
		return float4(lights[0][1] * (1.0 - (dist * (1.0 / pow(abs(mode), 0.5)))), 1.0 - (dist * (1.0 / pow(abs(mode), 0.5))));
		// return float4(lights[0][1] * (1.0 - dist) * (1.0 / abs(mode)), (1.0 - dist) * (1.0 / abs(mode)));
	}
	else if(modes[0] == 1) return float4(ambient, 1.0f); // ambient mode
	else if(modes[0] == 2) return float4(diffuse, 1.0f); // diffuse mode
	else if(modes[0] == 3) return float4(specular, 1.0f); // specular mode
	else if(modes[0] == 4){ // depth mode
		float depth = sqrt(pow(input.vertex_pos.x, 2) + pow(input.vertex_pos.y, 2) + pow(input.vertex_pos.z, 2)); // depth calculation
		return float4(depth, depth, depth, 1.0f);
	}
	else if(modes[0] == 5) return float4(lights[0][1], 1.0); // reference mode
	else if(modes[0] == 6) return float4(lights[0][1] * sin(dot(specular, diffuse)), 1.0); // trig mode
	else if(modes[0] == 7) return float4(ambient + pow(diffuse, 1.0 / specular), 1.0); // power mode
	else if(modes[0] == 8) return float4(normalize(lights[0][0]) - normalize(input.vertex_pos), 1.0); // inverse mode
	else return float4(ambient + diffuse + specular, 1.0); // all lighting
}