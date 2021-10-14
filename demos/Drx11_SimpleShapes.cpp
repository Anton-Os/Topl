#include "Canvas.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_Volumes.hpp" // shader inclusion
#include "Drx11_Flat.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Simple Shapes");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization

	Volumes_VertexShader vertexShader = Volumes_VertexShader();
	Flat_PixelShader fragmentShader = Flat_PixelShader();

	renderer.setCamera(&Topl::camera);
	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene, &Topl::camera);

	Main::gameLoop(&platform, &renderer);

	return 0;
}