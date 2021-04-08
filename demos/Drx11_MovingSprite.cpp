#include "Drx11_MovingSprite.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	NATIVE_WINDOW mainWindow = platform.createWindow("Moving Sprite");
	platform.setupMainWindow(mainWindow);
	Platform::keyLogger.addCallback('w', buttonCallback_w);
	Platform::keyLogger.addCallback('a', buttonCallback_a);
	Platform::keyLogger.addCallback('s', buttonCallback_s);
	Platform::keyLogger.addCallback('d', buttonCallback_d);

	Topl_Renderer_Drx11 renderer(mainWindow); // Renderer initialization

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Basic.hlsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string pixelShaderSrc = getParentDir(argv[0]) + "\\Pixel_Basic.hlsl";
	PixelShader pixelShader = PixelShader(pixelShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &pixelShader);

	renderer.buildScene(&Topl::sceneManager);

	while (renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&Topl::sceneManager);
		Topl::sceneManager.resolvePhysics();
		
		platform.handleEvents();
	}

	return 0;
}