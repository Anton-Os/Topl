#include "Assets.hpp"

#include "directx/Topl_Renderer_Drx11.hpp"
#include "Flat_Shader.hpp" // shader inclusion

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Assets");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization
	Topl_Pipeline_Drx11 pipeline; // Pipeline declaration

	Drx11_Flat_VertexShader vertexShader = Drx11_Flat_VertexShader(FLAT_MODE_ALTERNATE);
	Drx11_Flat_FragmentShader fragmentShader = Drx11_Flat_FragmentShader();
	renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);

	renderer.setCamera(&App::camera);
	renderer.buildScene(&App::scene, &App::camera);

	Main::gameLoop(&platform, &renderer);

	return 0;
}