#include "Blocks.hpp"

#include "directx/Topl_Renderer_Drx11.hpp"
#include "Flat_Shader.hpp"

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Simple Shapes");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization
	Topl_Pipeline_Drx11 pipeline; // Pipeline declaration

	Drx11_Flat_VertexShader vertexShader = Drx11_Flat_VertexShader();
	Drx11_Flat_FragmentShader fragmentShader = Drx11_Flat_FragmentShader();
	renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);

	renderer.setCamera(&App::camera);
	renderer.buildScene(&App::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}