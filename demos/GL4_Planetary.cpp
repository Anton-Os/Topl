#include "Planetary.hpp"

#include "Topl_Graphics_GL4.hpp"
#include "Flat_Shader.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Planetary");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());
	Topl_Pipeline_GL4 pipeline;

	GL4_Flat_VertexShader vertexShader = GL4_Flat_VertexShader();
	GL4_Flat_FragmentShader fragmentShader = GL4_Flat_FragmentShader();
	renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);

	renderer.setCamera(&Topl::camera);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}