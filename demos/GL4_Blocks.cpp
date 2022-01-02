#include "Blocks.hpp"

#include "Topl_Graphics_GL4.hpp"
#include "Flat_Shader.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Simple Shapes");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());

	GL4_Flat_VertexShader vertexShader = GL4_Flat_VertexShader();
	GL4_Flat_FragmentShader fragmentShader = GL4_Flat_FragmentShader();
	renderer.setPipeline(&vertexShader, &fragmentShader);

	renderer.setCamera(&Topl::camera);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}