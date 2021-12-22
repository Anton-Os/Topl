// #include "Drx11_MovingSprite.hpp"

#include "MovingSprite.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_Textured.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "Drx11 Moving Sprite");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization

	Textured_VertexShader vertexShader = Textured_VertexShader();
	Basic_PixelShader pixelShader = Basic_PixelShader();

	renderer.setCamera(&Topl::camera);
	renderer.setPipeline(&vertexShader, &pixelShader);
	renderer.buildScene(&Topl::scene, &Topl::camera);

	Main::gameLoop(&platform, &renderer);

	return 0;
}