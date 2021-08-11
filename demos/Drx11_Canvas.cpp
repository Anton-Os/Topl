#include "Canvas.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_FrameFx.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0]);
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow()); // Renderer initialization

	FrameFx_VertexShader vertexShader = FrameFx_VertexShader(&platform);
	FrameFx_PixelShader fragmentShader = FrameFx_PixelShader();

	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}