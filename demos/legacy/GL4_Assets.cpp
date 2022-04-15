#include "Assets.hpp"

#include "opengl/Topl_Renderer_GL4.hpp"
#include "Flat_Shader.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Assets");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());
	Topl_Pipeline_GL4 pipeline;

	GL4_Flat_VertexShader vertexShader = GL4_Flat_VertexShader(FLAT_MODE_ALTERNATE);
	GL4_Flat_FragmentShader fragmentShader = GL4_Flat_FragmentShader();
	renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);

	renderer.setCamera(&App::camera);
	renderer.buildScene(&App::scene);
	
	Main::gameLoop(&platform, &renderer);

	return 0;
}