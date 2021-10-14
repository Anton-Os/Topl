#include "Overlay.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_FrameFx.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Overlay");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization

	FrameFx_VertexShader vertexShader = FrameFx_VertexShader(&platform);
	FrameFx_PixelShader fragmentShader = FrameFx_PixelShader();

	renderer.setCamera(&Topl::camera);
	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene, &Topl::camera);

	Main::gameLoop(&platform, &renderer);

	return 0;
}