#include "Planetary.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "Flat_Shader.hpp"
#include "Beams_Shader.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Planetary");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());
	Topl_Pipeline_GL4 pipeline;

	GL4_Beams_VertexShader vertexBeams = GL4_Beams_VertexShader();
	GL4_Beams_FragmentShader fragBeams = GL4_Beams_FragmentShader();
	// renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);
	renderer.genPipeline(&pipeline, &vertexBeams, &fragBeams);

	renderer.setCamera(&App::camera);
	renderer.buildScene(&App::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}