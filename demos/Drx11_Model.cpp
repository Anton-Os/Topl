#include "Drx11_Model.hpp"

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	platform.createWindow("Model");

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow());
	Topl::scene.addGeometry(Topl::docTree.getNodeName(0), &Topl::geo); // gets the first node name

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.hlsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Pixel_Flat.hlsl";
	PixelShader fragmentShader = PixelShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(false, SpatialBounds3D(30.0f));

	Topl::model.log("Output.model"); // for testing

	Timer_Ticker gameTicker;
	while (renderer.renderScene(DRAW_Triangles)) {
		// Topl::geo.updateRot(Eigen::Vector2f(0.0, 0.001 * gameTicker.getAbsSecs()));

		renderer.updateScene(&Topl::scene);
		platform.handleEvents();
	}

	return 0;
}