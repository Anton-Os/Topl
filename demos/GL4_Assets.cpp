#include "Assets.hpp"

#include "Topl_Graphics_GL4.hpp"
#include "GL4_Flat.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Assets");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());
	Topl_Pipeline_GL4 pipeline;

	Flat_VertexShader vertexShader = Flat_VertexShader();
	Flat_FragmentShader fragmentShader = Flat_FragmentShader();
	renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);

	renderer.setCamera(&Topl::camera);
	renderer.buildScene(&Topl::scene, &Topl::camera);
	
	Main::gameLoop(&platform, &renderer);

	return 0;
}