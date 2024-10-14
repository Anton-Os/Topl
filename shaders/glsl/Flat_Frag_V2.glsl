#version 440

// Values

layout(std140, binding = 0) uniform Block {
// uniform Block {
	vec4 color;
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
};

layout(location = 1) uniform int modeTest;
layout(location = 2) uniform vec3 posTest;
layout(location = 3) uniform vec3 rotTest;
layout(location = 4) uniform vec3 sizeTest;
layout(location = 5) uniform vec4 colorTest;

layout(location = 0) in vec3 pos;
layout(location = 1) flat in int id;
layout(location = 2) in vec4 vert_color;

layout(location = 0) out vec4 outColor;

vec4 getRandColor(vec4 seedColor){
	vec4 randColor = seedColor * vec4(34.234, 11.559, 81.344, 1.0);

	while(randColor.x > 1.0) randColor.r -= pow(randColor.x, 0.5);
	while(randColor.y > 1.0) randColor.g -= pow(randColor.y, 0.5);
	while(randColor.z > 1.0) randColor.b -= pow(randColor.z, 0.5);

	return randColor;
}

vec4 getUniqueColor(uint index){
	float attenuation = floor(index / 6.0) * 0.025;

	if(index % 6 == 0) return vec4(1.0 - attenuation, 0.0, 0.0, 1.0); // red
	else if (index % 6 == 1) return vec4(0.0, 1.0 - attenuation, 0.0, 1.0); // green
	else if (index % 6 == 2) return vec4(0.0, 0.0, 1.0 - attenuation, 1.0); // blue
	else if (index % 6 == 3) return vec4(1.0 - attenuation, 1.0 - attenuation, 0.0, 1.0); // yellow
	else if (index % 6 == 4) return vec4(0.0, 1.0 - attenuation, 1.0 - attenuation, 1.0); // cyan
	else if (index % 6 == 5) return vec4(1.0 - attenuation, 0.0, 1.0 - attenuation, 1.0); // magenta
	else return vec4(1.0 - attenuation, 1.0 - attenuation, 1.0 - attenuation, 1.0); // white
}

// Main

void main() {
	uint primID = gl_PrimitiveID;

	if (mode == 1) // directional mode
		outColor = vec4((pos.x / 2.0) + 0.5, (pos.y / 2.0) + 0.5, (pos.z / 2.0) + 0.5, color.a);
	else if (mode == 2) // coordinate mode
		outColor = vec4((pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (pos.z - offset.z) * 2.0 + 0.5, color.a);
	else if (mode == 3) // random color mode
		outColor = getRandColor(color);
	// else if (mode == 4) // vertex mode
	//	outColor = vert_color;
	else if(mode == -1) // indexing mode
		outColor = getUniqueColor(primID);
	else if(mode < -1){
		float fraction = float(primID) / float(-mode);
		float level = 1.0 / float(-mode);

		/* uint i = 0;
		 *	for(float s = 0; s < 1.0; s += level){
		 *		if(fraction - floor(fraction) < s) outColor = getUniqueColor(i);
		 *		else break;
		 *		i++;
	} */

		if(fraction - floor(fraction) < 0.25) outColor = vec4(1.0, 0.0, 0.0, 0.8);
		else if(fraction - floor(fraction) < 0.5) outColor = vec4(0.0, 1.0, 0.0, 0.8);
		else if(fraction - floor(fraction) < 0.75) outColor = vec4(0.0, 0.0, 1.0, 0.8);
		else outColor = vec4(1.0, 1.0, 1.0, 0.8);
	}
	else outColor = color; // solid mode // default
}
