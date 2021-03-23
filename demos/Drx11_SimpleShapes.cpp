#include "Drx11_SimpleShapes.hpp"

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0]);
	NATIVE_WINDOW mainWindow = platform.createWindow("Simple Shapes");
	platform.setupMainWindow(mainWindow);

	Topl_Renderer_Drx11 renderer(mainWindow); // Renderer initialization

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.hlsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Flat.hlsl";
	PixelShader fragmentShader = PixelShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl::sceneManager);

	while ( renderer.renderScene(DRAW_Triangles)) {
		// renderer.updateScene(&Topl::sceneManager);
		// Topl::sceneManager.resolvePhysics();

		platform.handleEvents();
	}

	return 0;
}