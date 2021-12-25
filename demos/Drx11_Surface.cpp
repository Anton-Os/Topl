#include "Surface.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_Effect.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Surface");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow());

	Effect_VertexShader vertexShader = Effect_VertexShader(&platform);
	Effect_PixelShader fragmentShader = Effect_PixelShader();

	renderer.setCamera(&Topl::camera);
	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene, &Topl::camera);

	Main::gameLoop(&platform, &renderer);

	return 0;
}