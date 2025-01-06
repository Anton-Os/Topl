#version 440

#define INCLUDE_BLOCK
#define INCLUDE_TEXTURES
#define IGNORE_INPUTS

#include "Common.glsl"

// Values

layout(std140, binding = 1) uniform SceneBlock{
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	vec3 lightPos;
	vec3 lightVal;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vertex_pos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 texcoord;

layout(location = 0) out vec4 color;

// Functions

float calcSpec(vec3 light, vec3 vertex, float intensity) {
	vec3 reflectVec = light - (normalize(vertex) * 2 * dot(light, normalize(vertex)));
	if(mode < 0) return max(pow(dot(reflectVec, -normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z))), 1.0), 0);
	else return max(pow(dot(reflectVec, -normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z))), intensity), 0);
}

float calcDiffuse(vec3 light, vec3 vertex) {
	float intensity = dot(normalize(light), normalize(vertex));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

// Main

void main() {
	uvec4 modes = getModes(mode);
	uint intensity = modes[2] + 1;

	vec3 target;
	if(mode >= 0) target = normal; else target = vertex_pos; // set target conditionally

	vec3 texVals[3];
	texVals[0] = vec3(sampleTex_mode((abs(mode) + 1) % 8, texcoord)); // vec3(color_correct(texture(tex1, vec2(texcoord.x, texcoord.y))));
	texVals[1] = vec3(sampleTex_mode((abs(mode) + 2) % 8, texcoord)); // vec3(color_correct(texture(tex2, vec2(texcoord.x, texcoord.y))));
	texVals[2] = vec3(sampleTex_mode((abs(mode) + 3) % 8, texcoord)); // vec3(color_correct(texture(tex3, vec2(texcoord.x, texcoord.y))));

	// vec3 lightPos_diffuse = lightPos + (vec3(color_correct(texture(tex4, vec2(texcoord.x, texcoord.y)))) - vec3(0.5F, 0.5F, 0.5F));
	// vec3 lightPos_specular = lightPos + (vec3(color_correct(texture(tex5, vec2(texcoord.x, texcoord.y)))) - vec3(0.5F, 0.5F, 0.5F));

	color = color_correct(texture(baseTex, vec2(texcoord.x, texcoord.y)));
	vec3 ambient = ((lightVal + texVals[0]) / 2) * (0.25 + (0.05 * intensity));
	vec3 diffuse = ((lightVal + texVals[1]) / 2) * calcDiffuse(lightPos, target - offset) * 0.5 * intensity;
	vec3 specular = ((lightVal + texVals[2]) / 2) * calcSpec(lightPos, target, float(1 + intensity));

	// TODO: Calculate light interaction with texture

	color *= vec4(ambient + diffuse + specular, 1.0f); // all lighting
}
