#include "Drx11_MovingSprite.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	platform.createWindow("Moving Sprite");
	// platform.setupMainWindow(mainWindow);
	Platform::keyLogger.addCallback('w', buttonCallback_w);
	Platform::keyLogger.addCallback('a', buttonCallback_a);
	Platform::keyLogger.addCallback('s', buttonCallback_s);
	Platform::keyLogger.addCallback('d', buttonCallback_d);
	Platform::keyLogger.addCallback('r', buttonCallback_r);

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow()); // Renderer initialization

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Basic.hlsl";
	Basic_VertexShader vertexShader = Basic_VertexShader(vertexShaderSrc.c_str());
	std::string pixelShaderSrc = getParentDir(argv[0]) + "\\Pixel_Basic.hlsl";
	Basic_PixelShader pixelShader = Basic_PixelShader(pixelShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &pixelShader);

	renderer.buildScene(&Topl::scene);

	while (renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&Topl::scene);
		Topl::scene.resolvePhysics();
		
		platform.handleEvents();
	}

	return 0;
}