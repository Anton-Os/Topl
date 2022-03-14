#include "Surface.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Effect_Shader.hpp"
// #include "Drx11_Effect.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Surface");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization
	Topl_Pipeline_Drx11 pipeline; // Pipeline declaration

	Drx11_Effect_VertexShader vertexShader = Drx11_Effect_VertexShader();
	Drx11_Effect_FragmentShader fragmentShader = Drx11_Effect_FragmentShader();
	renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);

	renderer.setCamera(&App::camera);
	renderer.buildScene(&App::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}