#define IGNORE_INPUTS
#define INCLUDE_EXTBLOCK

#include "Common.hlsl"

struct VS_OUTPUT { float4 pos : SV_POSITION; }; // must match previous stage

[maxvertexcount(3)]
// void GSScene(triangleadj GSSceneIn input[6], inout TriangleStream<VS_OUTPUT> outStream) {
void main(triangle VS_OUTPUT input[3], inout TriangleStream<VS_OUTPUT> outStream) {
	VS_OUTPUT outVertex = (VS_OUTPUT)0;

	for (uint i = 0; i < 3; i++) {
		outVertex.pos = input[i].pos * float4(2.0f, -2.0f, 0.0f, 0.0f); // stretch by x2 and invert Y

		outStream.Append(outVertex);
	}

	outStream.RestartStrip();
}