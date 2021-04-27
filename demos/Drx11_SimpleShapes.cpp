#include "Drx11_SimpleShapes.hpp"

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0]);
	platform.createWindow("Simple Shapes");
	//platform.setupMainWindow(mainWindow);
	Platform::keyLogger.addCallback('w', buttonCallback_w);
	Platform::keyLogger.addCallback('a', buttonCallback_a);
	Platform::keyLogger.addCallback('s', buttonCallback_s);
	Platform::keyLogger.addCallback('d', buttonCallback_d);

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow()); // Renderer initialization

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.hlsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Pixel_Flat.hlsl";
	PixelShader fragmentShader = PixelShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(false, SpatialBounds3D(3.0f));

	Timer_Ticker gameTicker;
	// Topl::grid.move(&Topl::scene, Eigen::Vector3f(1.0f, -100.0f, 0.0f));
	while (renderer.renderScene(DRAW_Triangles)) {
		Topl::chain.rotate(&Topl::scene, Eigen::Vector2f(-0.001 * gameTicker.getAbsSecs(), 0.0));
		Topl::grid.rotate(&Topl::scene, Eigen::Vector2f(0.0, 0.002 * gameTicker.getAbsSecs()));
		renderer.updateScene(&Topl::scene);
		// Topl::scene.resolvePhysics();

		platform.handleEvents();
	}

	return 0;
}