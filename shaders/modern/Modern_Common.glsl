// Modern Common: uniform blocks, stage varyings and math shared by every modern pipeline

#include <Enable_Textures>

// Actor block, matches Modern_VertexShader::genActorBlock()
layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec3 rotation;
	vec3 scale;

	vec4 actor_color; // rgb tint and alpha
	vec4 actor_params; // per-actor values, meaning depends on the pipeline
	vec4 actor_tex; // x texture blend, y tiling, z scroll speed, w mapping (0 mesh coordinates, 1 world XZ, 2 sprite with texture alpha)
};

#ifdef INCLUDE_TEXTURES
layout(binding = 0) uniform sampler2D baseTex; // per-actor texture chosen from the texture pane
#endif

// Scene block, matches Modern_VertexShader::genSceneBlock()
layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos; // xyz camera position, w zoom
	vec3 look_pos; // FPS camera angles: x yaw, y pitch, z roll
	mat4 projMatrix; // unused by modern shaders, kept for engine layout compatibility

	vec4 cam_lens; // x vertical fov in radians, y aspect ratio, z near plane, w far plane
	vec4 time_info; // x elapsed seconds, y frame seconds, z frame count
	vec4 light_pos; // xyz position (or direction when w < 0), abs(w) intensity
	vec4 light_color; // rgb light color, w ambient strength
	vec4 fog_color; // rgb fog and horizon color, w fog density
	vec4 params[8]; // pipeline specific parameters
};

// Varyings shared between vertex and fragment stages

#ifdef STAGE_VERTEX
	#define VARYING out
#else
	#define VARYING in
#endif

layout(location = 0) VARYING vec3 world_pos;
layout(location = 1) VARYING vec3 world_normal;
layout(location = 2) VARYING vec3 texcoord;
layout(location = 3) VARYING vec3 vertex_color;
layout(location = 4) VARYING vec3 view_ray; // camera to vertex vector, or ray direction for fullscreen passes
layout(location = 5) VARYING vec4 extra; // pipeline specific data

// Constants

#define TWO_PI 6.283185307
#define PI 3.141592653
#define HALF_PI 1.570796327

// Helpers

uvec4 getModes(int m){
	uint a = uint(abs(m));
	return uvec4(a % 10u, (a / 10u) % 10u, (a / 100u) % 10u, a / 1000u);
}

float getTime(){ return time_info.x; }

vec3 rotateX(vec3 v, float a){ float c = cos(a), s = sin(a); return vec3(v.x, c * v.y - s * v.z, s * v.y + c * v.z); }
vec3 rotateY(vec3 v, float a){ float c = cos(a), s = sin(a); return vec3(c * v.x - s * v.z, v.y, s * v.x + c * v.z); }
vec3 rotateZ(vec3 v, float a){ float c = cos(a), s = sin(a); return vec3(c * v.x - s * v.y, s * v.x + c * v.y, v.z); }

// Same axis convention as legacy getRotMatrix(): x rolls around Z, y pitches around X, z yaws around Y
vec3 actorRotate(vec3 v, vec3 angles){ return rotateZ(rotateY(rotateX(v, angles.y), angles.z), angles.x); }

vec3 hsv2rgb(vec3 c){
	vec3 p = abs(fract(c.xxx + vec3(1.0, 2.0 / 3.0, 1.0 / 3.0)) * 6.0 - 3.0);
	return c.z * mix(vec3(1.0), clamp(p - 1.0, 0.0, 1.0), c.y);
}

// Noise

float hash12(vec2 p){
	vec3 p3 = fract(vec3(p.x, p.y, p.x) * 0.1031);
	p3 += dot(p3, vec3(p3.y, p3.z, p3.x) + 33.33);
	return fract((p3.x + p3.y) * p3.z);
}

float hash13(vec3 p){
	vec3 p3 = fract(p * 0.1031);
	p3 += dot(p3, vec3(p3.z, p3.y, p3.x) + 31.32);
	return fract((p3.x + p3.y) * p3.z);
}

float valueNoise(vec2 p){
	vec2 i = floor(p);
	vec2 f = fract(p);
	vec2 u = f * f * (3.0 - 2.0 * f);
	float a = hash12(i);
	float b = hash12(i + vec2(1.0, 0.0));
	float c = hash12(i + vec2(0.0, 1.0));
	float d = hash12(i + vec2(1.0, 1.0));
	return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p){ // fractal brownian motion in range 0 to 1
	float total = 0.0, amp = 0.5, norm = 0.0;
	for(int o = 0; o < 5; o++){
		total += valueNoise(p) * amp;
		norm += amp;
		p = vec2(0.8 * p.x - 0.6 * p.y, 0.6 * p.x + 0.8 * p.y) * 2.03;
		amp *= 0.5;
	}
	return total / norm;
}
