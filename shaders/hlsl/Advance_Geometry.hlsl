[maxvertexcount(3)]
// void GSScene(triangleadj GSSceneIn input[6], inout TriangleStream<PSSceneIn> outStream) {
void GSScene(triangle GSSceneIn input[3], inout TriangleStream<PSSceneIn> outStream) {
	PSSceneIn outVertex = (PSSceneIn)0;

	for (uint i = 0; i < 3; i++) {
		outVertex.Pos = input[i].Pos;
		outVertex.Norm = input[i].Norm;
		outVertex.Tex = input[i].Tex;

		outStream.Append(outVertex);
	}

	outStream.RestartStrip();
}