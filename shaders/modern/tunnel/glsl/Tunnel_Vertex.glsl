#version 440

#define STAGE_VERTEX

#include "Modern_Common.glsl"

#include "Modern_Vertex.glsl"

// Fullscreen pass, every pixel receives a ray built from the FPS camera basis

void main() {
	passFullscreen();
}
