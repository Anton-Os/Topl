#version 440

// Values

layout(binding = 0) uniform sampler2D baseTex;
layout(binding = 1) uniform sampler2D tex1; // height?
layout(binding = 2) uniform sampler2D tex2; // roughness?
layout(binding = 3) uniform sampler2D tex3; // opacity?
layout(binding = 4) uniform sampler2D tex4; // enviornment?
layout(binding = 5) uniform sampler2D tex5; // shadow?
layout(binding = 6) uniform sampler2D tex6; // illumination?
layout(binding = 7) uniform sampler2D tex7; // testing?
layout(binding = 8) uniform sampler3D volumeTex;

layout(location = 0) in vec3 texcoord;
layout(location = 1) flat in int mode;

layout(location = 0) out vec4 color;

// Funtions

/* vec4 color_layer(vec4 color, vec4 oldColor){
	if(color.a < oldColor.a) return oldColor;
	else return vec4((color.r * color.a) + (oldColor.r * (1 - color.a)), (color.g * color.a) + (oldColor.g * (1 - color.a)), (color.b * color.a) + (oldColor.b * (1 - color.a)), color.a);
} */

vec4 color_correct(vec4 color){ // switch red and blue color values
	float t = color.r;
	color.r = color.b; color.b = t;
	return color;
}

// Main

void main() {
	if(mode == 8) color = color_correct(texture(volumeTex, texcoord)); // volumetric texture
	else if(mode > 0 && mode < 8){
		if(mode == 1) color = color_correct(texture(tex1, vec2(texcoord.x, texcoord.y)));
		else if(mode == 2) color = color_correct(texture(tex2, vec2(texcoord.x, texcoord.y)));
		else if(mode == 3) color = color_correct(texture(tex3, vec2(texcoord.x, texcoord.y)));
		else if(mode == 4) color = color_correct(texture(tex4, vec2(texcoord.x, texcoord.y)));
		else if(mode == 5) color = color_correct(texture(tex5, vec2(texcoord.x, texcoord.y)));
		else if(mode == 6) color = color_correct(texture(tex6, vec2(texcoord.x, texcoord.y)));
		else if(mode == 7) color = color_correct(texture(tex7, vec2(texcoord.x, texcoord.y)));
		else color = vec4(1.0, 0.0, 0.0, 1.0); // error
	}
	else if(mode < 0 && mode > -8){ // Alpha Values go to top
		if(mode <= -1 && texture(tex1, vec2(texcoord.x, texcoord.y)).a >= color.a) 
			color = texture(tex1, vec2(texcoord.x, texcoord.y));
		if(mode <= -2 && texture(tex2, vec2(texcoord.x, texcoord.y)).a >= color.a) 
			color = texture(tex2, vec2(texcoord.x, texcoord.y));
		if(mode <= -3 && texture(tex3, vec2(texcoord.x, texcoord.y)).a >= color.a) 
			color = texture(tex3, vec2(texcoord.x, texcoord.y));
		if(mode <= -4 && texture(tex4, vec2(texcoord.x, texcoord.y)).a >= color.a) 
			color = texture(tex4, vec2(texcoord.x, texcoord.y));
		if(mode <= -5 && texture(tex5, vec2(texcoord.x, texcoord.y)).a >= color.a) 
			color = texture(tex5, vec2(texcoord.x, texcoord.y));
		if(mode <= -6 && texture(tex6, vec2(texcoord.x, texcoord.y)).a >= color.a) 
			color = texture(tex6, vec2(texcoord.x, texcoord.y));
		if(mode <= -7 && texture(tex7, vec2(texcoord.x, texcoord.y)).a >= color.a) 
			color = texture(tex7, vec2(texcoord.x, texcoord.y));
	}
	else color = color_correct(texture(baseTex, vec2(texcoord.x, texcoord.y))); // base texture

	if (color.a < 0.05) discard; // blending fix
}
