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
	
	renderer.buildScene(&Topl::sceneManager);

	while ( renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&Topl::sceneManager);
		// Topl::sceneManager.resolvePhysics();

		platform.handleEvents();
	}

	return 0;
}