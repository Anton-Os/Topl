#include "Planetary.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_Volumes.hpp" // shader inclusion
#include "Drx11_Flat.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0]);
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow()); // Renderer initialization

	Volumes_VertexShader vertexShader = Volumes_VertexShader();
	Flat_PixelShader fragmentShader = Flat_PixelShader();

	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}