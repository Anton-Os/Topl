#define IGNORE_INPUTS
#define INCLUDE_EXTBLOCK

#define STAGE_GEOMETRY

#include "Common.hlsl"

#include "Extended.hlsl"

#define GEOM_MAIN_ENTRY void main(point GS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> outStream) // TODO: Define this inside Advance_Shader.hpp

struct GS_OUTPUT { float4 pos : SV_POSITION; }; // must match previous stage

[maxvertexcount(3)]
// void GSScene(triangleadj GSSceneIn input[6], inout TriangleStream<GS_OUTPUT> outStream) {
// void main(triangle GS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outStream) {
GEOM_MAIN_ENTRY {
	GS_OUTPUT outVertex = (GS_OUTPUT)0;

	/* for (uint i = 0; i < 3; i++) {
		outVertex.pos = input[i].pos * float4(2.0f, -2.0f, 0.0f, 0.0f); // stretch by x2 and invert Y

		outStream.Append(outVertex);
	} */

	outVertex.pos = input[0].pos;
	outStream.Append(outVertex);

	outVertex.pos = input[0].pos + float4(0.02, -0.05, 0.0, 0.0);
	outStream.Append(outVertex);
	
	outVertex.pos = input[0].pos + float4(-0.05, 0.02, 0.0, 0.0);
	outStream.Append(outVertex);

	outStream.RestartStrip();
}