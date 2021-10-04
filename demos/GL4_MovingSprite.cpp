#include "MovingSprite.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "GL4_Basic.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Moving Sprite");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());

	Basic_VertexShader vertexShader = Basic_VertexShader();
	Basic_FragmentShader fragmentShader = Basic_FragmentShader();

	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}