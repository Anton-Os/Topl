#version 440

#define INCLUDE_TEXTURES

#include "Common.glsl"

#include "Pixel.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	// uint actorID;
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	double timeFrame;
	double timeElapse;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vertex_pos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 vertex_color;
layout(location = 4) in vec3 texcoord;
layout(location = 5) in vec3 tangent;
layout(location = 6) flat in int id;

layout(location = 0) out vec4 color_final;

// Functions

/* vec3 tagPattern(int vertex_id, vec3 coords){
	if(vertex_id % 3 == 0) return vec3(sin(coords.x), cos(coords.y), tan(coords.z));
	else if(vertex_id % 3 == 1) return vec3(sin(-coords.x), cos(-coords.y), tan(-coords.z));
	else return coords * (1.0 / vertex_id);
} */

vec3 solidPattern1(vec3 coords, uint m){
	float r = ((coords.x * 5) - floor(coords.x * 5)) * m; // pow(coords.x, coords.y) * m;
	float g = ((coords.y * 10) - floor(coords.y * 10)) * m; // pow(coords.y, coords.z) * m;
	float b = ((coords.z * 50) - floor(coords.z * 50)) * m; // pow(coords.z, coords.x) * m;

	return vec3(abs(r) - floor(abs(r)), abs(g) - floor(abs(g)), abs(b) - floor(abs(b)));
}

vec3 solidPattern2(vec3 coords, uint m){
	float r = pow(abs(coords.x + coords.z), abs(coords.y)) * m;
	float g = pow(abs(coords.z - coords.y), abs(coords.x)) * m;
	float b = pow(abs(coords.y * coords.x), abs(coords.z)) * m;

	return vec3(abs(r) - floor(abs(r)), abs(g) - floor(abs(g)), abs(b) - floor(abs(b)));
}

vec3 solidPattern3(vec3 coords, uint m){
	float r = abs(sin(coords.x * 2) + sin(coords.y * 5) + sin(coords.z * 10)) * m;
	float g = abs(cos(coords.y * 2) * cos(coords.z * 5) * cos(coords.x * 10)) * m;
	float b = pow(pow(tan(coords.z * 2), tan(coords.z * 5)), tan(coords.y * 10)) * m;

	return vec3(abs(r) - floor(abs(r)), abs(g) - floor(abs(g)), abs(b) - floor(abs(b)));
}

vec3 texturePattern1(vec3 coords, uint m, float i){
	vec3 intervals = vec3(floor(abs(coords.x * i)), floor(abs(coords.y * i)), floor(abs(coords.z * i))) / i;

	vec4 texColor = modalTex(int(m), coords - intervals) * length(coords - intervals);
	return vec3(texColor) * 2;
}

vec3 texturePattern2(vec3 coords, uint m, float i){
	vec3 intervals1 = vec3(floor(abs(coords.x * i)), floor(abs(coords.y * i)), floor(abs(coords.z * i))) / i;
	vec3 intervals2 = vec3(floor(abs(intervals1.y * (i * 5))), floor(abs(intervals1.z * (i * 5))), floor(abs(intervals1.x * (i * 5)))) / (i * 5);
	vec3 intervals3 = vec3(floor(abs(intervals2.z * (i * 10))), floor(abs(intervals2.x * (i * 10))), floor(abs(intervals2.y * (i * 10)))) / (i * 10);

	vec4 texColor = modalTex(int(m), intervals3);
	return vec3(abs(texColor.r), abs(texColor.g), abs(texColor.b)) * 2;
}

vec3 weavePattern1(vec3 coords){
	vec3 weave = vec3(
		(coords.x * coords.x) + abs(coords.y * coords.x) + (coords.y * coords.y),
		(coords.z * coords.z) + abs(coords.z * coords.x) + (coords.x * coords.x),
		(coords.y * coords.y) + abs(coords.y * coords.z) + (coords.z * coords.z)
	);

	return weave;
}

vec3 weavePattern2(vec3 coords, uint m){
	vec3 weave = vec3(
		pow(coords.x, m) + abs(coords.y * coords.x * m) + pow(coords.y, m),
		pow(coords.z, m) + abs(coords.z * coords.x * m) + pow(coords.x, m),
		pow(coords.y, m) + abs(coords.y * coords.z * m) + pow(coords.z, m)
	);

	return weave;
}

vec4 portalPattern1(vec3 coords, uint m, double t){
	vec4 texColor1 = modalTex(int(m), coords * sin(float(t)));
	vec4 texColor2 = modalTex(int(m + 1), coords * cos(float(t)));
	vec4 texColor3 = modalTex(int(m + 2), coords * tan(float(t)));

	return (texColor1 + texColor2) * texColor3; // TODO: Make this a cool effect
}

vec4 portalPattern2(vec3 coords, uint m, double t){
	vec4 texColor1 = modalTex(int(m), coords * sin(float(t)));
	vec4 texColor2 = modalTex(int(m + 1), coords * cos(float(t)));
	vec4 texColor3 = modalTex(int(m + 2), coords * tan(float(t)));

	return texColor3 * dot(texColor1, texColor2);
}

// Main

void main() {
	vec3 coords = pos;

	if(mode % 10 == 1) coords = vec3(vertex_color);
	else if(mode % 10 == 2) coords = normal;
	else if(mode % 10 == 3) coords = tangent;
	else if(mode % 10 == 4) coords = texcoord;
	else if(mode % 10 == 5) coords = getRandColor(id);
	else if(mode % 10 == 6) coords = vertex_pos / texcoord - normal * tangent;
	else if(mode % 10 == 7) coords = vec3(sin(texcoord.x / pos.x), cos(normal.y / pos.y), tan(tangent.z / pos.z));
	else if(mode % 10 == 8) coords = vec3(pow(abs(pos.x), abs(vertex_color.r)), pow(abs(pos.y), abs(texcoord.y)), pow(abs(pos.z), float(id)));
	else if(mode % 10 == 9) coords = vec3(dot(vertex_pos, vec3(vertex_color)), dot(getRandColor(uint(id)), vec3(getStepColor(uint(id)))), dot(texcoord, tangent));

	uint m = mode % 10; // uint(id);
	if(mode < 0) m = uint(id);
	// double t = timeElapse / 5000.0;
	double t = sin(float(timeElapse) / 5000.0) * (timeElapse / 30000.0);

	if(abs(mode / 100) % 10 == 1) color_final = vec4(solidPattern1(coords, m + 1), 1.0);
	else if(abs(mode / 100) % 10 == 2) color_final = vec4(solidPattern2(coords, m + 1), 1.0);
	else if(abs(mode / 100) % 10 == 3) color_final = vec4(solidPattern3(coords, m + 1), 1.0);
	else if(abs(mode / 100) % 10 == 4) color_final = vec4(texturePattern1(coords, m, (mode % 100) / 5), 1.0);
	else if(abs(mode / 100) % 10 == 5) color_final = vec4(texturePattern2(coords, m, (mode % 100) / 5), 1.0);
	else if(abs(mode / 100) % 10 == 6) color_final = vec4(weavePattern1(coords), 1.0);
	else if(abs(mode / 100) % 10 == 7) color_final = vec4(weavePattern2(coords, uint((mode % 100) / 10)), 1.0);
	else if(abs(mode / 100) % 10 == 8) color_final = portalPattern1(coords, m, t);
	else if(abs(mode / 100) % 10 == 9) color_final = portalPattern2(coords, m, t);
	// else if(abs(mode / 100) % 10 == 8) color_final = vec4(texturePattern1(weavePattern1(coords), m, (mode % 100) / 5), 1.0);
	// else if(abs(mode / 100) % 10 == 9) color_final = vec4(weavePattern2(texturePattern2(coords, m, (mode % 100) / 5), uint((mode % 100) / 10)), 1.0);
	
	else color_final = vec4(coords, 1.0);

	color_final = vec4(
		abs(color_final.r) - floor(abs(color_final.r)), 
		abs(color_final.g) - floor(abs(color_final.g)), 
		abs(color_final.b) - floor(abs(color_final.b)), 
		color_final.a
	);
}
