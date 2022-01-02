#include "Planetary.hpp"

#include "Topl_Graphics_Drx11.hpp"
#include "Flat_Shader.hpp"

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Planetary");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization

	Drx11_Flat_VertexShader vertexShader = Drx11_Flat_VertexShader();
	Drx11_Flat_FragmentShader fragmentShader = Drx11_Flat_FragmentShader();
	Topl_Pipeline_Drx11 pipeline = renderer.genPipeline(&vertexShader, &fragmentShader);
	// renderer.setPipeline(&pipeline);
	renderer.setPipeline(&vertexShader, &fragmentShader);

	renderer.setCamera(&Topl::camera);
	renderer.buildScene(&Topl::scene, &Topl::camera);

	Main::gameLoop(&platform, &renderer);

	return 0;
}