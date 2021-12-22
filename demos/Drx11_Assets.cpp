#include "Assets.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_Flat.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Assets");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization

	Flat_VertexShader vertexShader = Flat_VertexShader();
	Flat_PixelShader fragmentShader = Flat_PixelShader();

	renderer.setCamera(&Topl::camera);
	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene, &Topl::camera);

	Main::gameLoop(&platform, &renderer);

	return 0;
}