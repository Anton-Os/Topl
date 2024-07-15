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

layout(location = 2) uniform vec3 posTest;

layout(location = 0) in vec3 pos;
layout(location = 1) flat in int id;

layout(location = 0) out vec4 outColor;

// Main

void main() { 
	if (mode == 1) // directional
		outColor = vec4((pos.x / 2.0) + 0.5, (pos.y / 2.0) + 0.5, (pos.z / 2.0) + 0.5, color.a);
	else if (mode == 2) // coordinate mode
		outColor = vec4((pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (pos.z - offset.z) * 2.0 + 0.5, color.a);
	else if(mode < 0){
		// int remainder = id - ((-mode) * int(floor(float(id) / float(-mode))));
		float factor = 3.0;
		float fraction = float(id) / float(-mode);

		if(fraction - floor(fraction) < 0.25) outColor = vec4(1.0, 0.0, 0.0, 1.0);
		else if(fraction - floor(fraction) < 0.5) outColor = vec4(0.0, 1.0, 0.0, 1.0);
		else if(fraction - floor(fraction) < 0.75) outColor = vec4(0.0, 0.0, 1.0, 1.0);
		else outColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else {
		if(posTest == vec3(0.0, 0.0, 0.0))
			outColor = vec4(1.0, 0.0, 0.0, 1.0);
		else outColor = vec4(posTest[0], posTest[1], posTest[2], 1.0);
		/*else if(posTest == vec3(1.0, 1.0, 1.0))
			outColor = vec4(0.0, 1.0, 0.0, 1.0);
		else outColor = color; // solid mode // default */
	}
	// if(color == vec4(0.0, 0.0, 0.0, 0.0)) // no color
}
