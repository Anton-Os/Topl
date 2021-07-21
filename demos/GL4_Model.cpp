#include "GL4_Model.hpp"

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	platform.createWindow("Model");

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());
	Topl::scene.addGeometry(Topl::docTree.getNodeName(0), &Topl::geo); // gets the first node name

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Playdough.glsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Playdough.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(false, SpatialBounds3D(30.0f));

	Topl::model.log("Output.model"); // for testing

	glPointSize(5.0f); // TODO: include as part of the renderer code
	glLineWidth(3.0f); // TODO: include as part of the renderer code
	// Topl::geo.updatePos(Eigen::Vector3f(0.0f, -40.0f, 0.0f));

	Timer_Ticker gameTicker;
	while (renderer.renderScene(DRAW_Triangles)) {
		Topl::geo.updateRot(Eigen::Vector2f(0.0, 0.001 * gameTicker.getAbsSecs()));
		// Topl::geo.updateRot(Eigen::Vector2f(0.001 * gameTicker.getAbsSecs(), 0.0f));

		renderer.updateScene(&Topl::scene);
		platform.handleEvents();
	}

	return 0;
}