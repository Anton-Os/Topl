#version 440

#define STAGE_VERTEX

#include "Modern_Common.glsl"

#include "Modern_Vertex.glsl"

// Fullscreen sky pass, rays are generated from the FPS camera basis

void main() {
	passFullscreen();
}
