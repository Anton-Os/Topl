// #include "Drx11_MovingSprite.hpp"

#include "MovingSprite.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_Basic.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow()); // Renderer initialization

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Basic.hlsl";
	Basic_VertexShader vertexShader = Basic_VertexShader(vertexShaderSrc.c_str());
	std::string pixelShaderSrc = getParentDir(argv[0]) + "\\Pixel_Basic.hlsl";
	Basic_PixelShader pixelShader = Basic_PixelShader(pixelShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &pixelShader);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}