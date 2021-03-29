#include "GL4_MovingSprite.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	NATIVE_WINDOW mainWindow = platform.createWindow("Moving Sprite");
	platform.setupMainWindow(mainWindow);
	Platform::keyLogger.addCallback('w', buttonCallback_w);

	Topl_Renderer_GL4 renderer(mainWindow);

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Basic.glsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Basic.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl::sceneManager);

	while ( renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&Topl::sceneManager); // Uncomment
		Topl::sceneManager.resolvePhysics(); // Uncomment

		platform.handleEvents();
	}

	return 0;
}