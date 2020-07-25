#version 440

layout(std140) uniform Block{
	vec3 offset;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 texcoord_out;
layout(location = 4) out vec4 hardColor_out;

vec4 colorStart = vec4(0.22, 0.1, 0.33, 1.0);
vec4 colorWheel1 = vec4(0.22, 0.88, 0.77, 1.0);
vec4 colorWheel2 = vec4(0.55, 0.88, 0.55, 1.0);
vec4 colorWheel3 = vec4(0.66, 0.22, 0.33, 1.0);
vec4 colorWheel4 = vec4(0.77, 0.44, 0.11, 1.0);
vec4 colorWheel5 = vec4(0.44, 0.77, 0.11, 1.0);

void main() {
	texcoord_out = texcoord;

	uint vertexID_mod5 = gl_VertexID % 5;
	if(gl_VertexID == 0) hardColor_out = colorStart;
	else 
		switch(vertexID_mod5){
			case 0:
			hardColor_out = colorWheel1;
			break;
			case 1:
			hardColor_out = colorWheel2;
			break;
			case 2:
			hardColor_out = colorWheel3;
			break;
			case 3:
			hardColor_out = colorWheel4;
			break;
			case 4:
			hardColor_out = colorWheel5;
			break;
				
		}

	gl_Position = vec4(pos + offset, 1.0);
}