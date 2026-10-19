// Modern Common: constant buffers, stage structures and math shared by every modern pipeline

#include <Enable_Textures>

// Actor block, matches Modern_VertexShader::genActorBlock()
cbuffer CONST_BLOCK : register(b0) {
	float3 offset;
	float3 rotation;
	float3 scale;

	float4 actor_color; // rgb tint and alpha
	float4 actor_params; // per-actor values, meaning depends on the pipeline
	float4 actor_tex; // x texture blend, y tiling, z scroll speed, w mapping (0 mesh coordinates, 1 world XZ, 2 sprite with texture alpha)
}

#ifdef INCLUDE_TEXTURES
Texture2D baseTex : register(t0); // per-actor texture chosen from the texture pane
SamplerState baseSampler : register(s0);
#endif

// Scene block, matches Modern_VertexShader::genSceneBlock()
cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos; // xyz camera position, w zoom
	float4 look_pos; // FPS camera angles: x yaw, y pitch, z roll
	float4x4 projMatrix; // unused by modern shaders, kept for engine layout compatibility

	float4 cam_lens; // x vertical fov in radians, y aspect ratio, z near plane, w far plane
	float4 time_info; // x elapsed seconds, y frame seconds, z frame count
	float4 light_pos; // xyz position (or direction when w < 0), abs(w) intensity
	float4 light_color; // rgb light color, w ambient strength
	float4 fog_color; // rgb fog and horizon color, w fog density
	float4 params[8]; // pipeline specific parameters
}

// Stage structures

struct VS_INPUT {
	float4 pos : POSITION;
	float3 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 vert_color : COLOR;
	float3 tangent : TANGENT;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 world_pos : POSITION1;
	float3 world_normal : NORMAL;
	float3 texcoord : TEXCOORD0;
	float3 vertex_color : COLOR;
	float3 view_ray : TEXCOORD1; // camera to vertex vector, or ray direction for fullscreen passes
	float4 extra : TEXCOORD2; // pipeline specific data
};

typedef VS_OUTPUT PS_INPUT;

// Constants

#define TWO_PI 6.283185307
#define PI 3.141592653
#define HALF_PI 1.570796327

// Helpers

uint4 getModes(int m){
	uint a = uint(abs(m));
	return uint4(a % 10u, (a / 10u) % 10u, (a / 100u) % 10u, a / 1000u);
}

float getTime(){ return time_info.x; }

float modulo(float x, float y){ return x - y * floor(x / y); } // GLSL style mod for negative values
float2 modulo(float2 x, float2 y){ return x - y * floor(x / y); }

float3 rotateX(float3 v, float a){ float c = cos(a), s = sin(a); return float3(v.x, c * v.y - s * v.z, s * v.y + c * v.z); }
float3 rotateY(float3 v, float a){ float c = cos(a), s = sin(a); return float3(c * v.x - s * v.z, v.y, s * v.x + c * v.z); }
float3 rotateZ(float3 v, float a){ float c = cos(a), s = sin(a); return float3(c * v.x - s * v.y, s * v.x + c * v.y, v.z); }

// Same axis convention as the GLSL version: x rolls around Z, y pitches around X, z yaws around Y
float3 actorRotate(float3 v, float3 angles){ return rotateZ(rotateY(rotateX(v, angles.y), angles.z), angles.x); }

float3 hsv2rgb(float3 c){
	float3 p = abs(frac(c.xxx + float3(1.0, 2.0 / 3.0, 1.0 / 3.0)) * 6.0 - 3.0);
	return c.z * lerp(float3(1.0, 1.0, 1.0), saturate(p - 1.0), c.y);
}

// Noise

float hash12(float2 p){
	float3 p3 = frac(float3(p.x, p.y, p.x) * 0.1031);
	p3 += dot(p3, float3(p3.y, p3.z, p3.x) + 33.33);
	return frac((p3.x + p3.y) * p3.z);
}

float hash13(float3 p){
	float3 p3 = frac(p * 0.1031);
	p3 += dot(p3, float3(p3.z, p3.y, p3.x) + 31.32);
	return frac((p3.x + p3.y) * p3.z);
}

float valueNoise(float2 p){
	float2 i = floor(p);
	float2 f = frac(p);
	float2 u = f * f * (3.0 - 2.0 * f);
	float a = hash12(i);
	float b = hash12(i + float2(1.0, 0.0));
	float c = hash12(i + float2(0.0, 1.0));
	float d = hash12(i + float2(1.0, 1.0));
	return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

float fbm(float2 p){ // fractal brownian motion in range 0 to 1
	float total = 0.0, amp = 0.5, norm = 0.0;
	[unroll] for(int o = 0; o < 5; o++){
		total += valueNoise(p) * amp;
		norm += amp;
		p = float2(0.8 * p.x - 0.6 * p.y, 0.6 * p.x + 0.8 * p.y) * 2.03;
		amp *= 0.5;
	}
	return total / norm;
}
