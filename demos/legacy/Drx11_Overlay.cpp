#include "Overlay.hpp"

#include "directx/Topl_Renderer_Drx11.hpp"
#include "Textured_Shader.hpp"
#include "Flat_Shader.hpp"

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Overlay");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization
	Topl_Pipeline_Drx11 pipeline; // Pipeline declaration

	// Drx11_Flat_VertexShader vertexShader = Drx11_Flat_VertexShader();
	Drx11_Textured_VertexShader vertexShader = Drx11_Textured_VertexShader();
	// Drx11_Flat_FragmentShader fragmentShader = Drx11_Flat_FragmentShader();
	Drx11_Textured_FragmentShader fragmentShader = Drx11_Textured_FragmentShader();
	renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);

	renderer.setCamera(&App::camera);
	renderer.buildScene(&App::scene);

	Main::gameLoop(&platform, &renderer);

	cleanupFreeType(&App::freetypeLib);
	return 0;
}