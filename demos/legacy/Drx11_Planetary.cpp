#include "Planetary.hpp"

#include "directx/Topl_Renderer_Drx11.hpp"
#include "Flat_Shader.hpp"
#include "Beams_Shader.hpp"

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Planetary");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization
	Topl_Pipeline_Drx11 pipeline; // Pipeline declaration

	Drx11_Beams_VertexShader vertexBeams = Drx11_Beams_VertexShader();
	Drx11_Beams_FragmentShader fragBeams = Drx11_Beams_FragmentShader();
	// renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);
	renderer.genPipeline(&pipeline, &vertexBeams, &fragBeams);

	renderer.setCamera(&App::camera);
	renderer.buildScene(&App::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}