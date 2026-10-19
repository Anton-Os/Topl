#include "Modern_Common.hlsl"

#include "Modern_Vertex.hlsl"

// Fullscreen pass, every pixel receives a ray built from the FPS camera basis

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	return passFullscreen(input);
}
