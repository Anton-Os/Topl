#version 440

#include "Modern_Common.glsl"

#include "Modern_Frag.glsl"

// Main: writes the id color untouched by lighting, fog or tonemapping

void main() {
	color_final = vec4(actor_color.rgb, 1.0);
}
