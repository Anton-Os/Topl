#include "GL4_SimpleShapes.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	NATIVE_WINDOW mainWindow = platform.createWindow("Simple Shapes");
	platform.setupMainWindow(mainWindow);

	Topl_Renderer_GL4 renderer(mainWindow);

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.glsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Flat.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl::sceneManager);

	glPointSize(5.0f); // TODO: Include as part of the renderer code
	while ( renderer.renderScene(DRAW_Points)) {
		// renderer.updateScene(&Topl::sceneManager);
		// Topl::sceneManager.resolvePhysics();

		platform.handleEvents();
	}

	return 0;
}