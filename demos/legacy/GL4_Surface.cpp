#include "Surface.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "Effect_Shader.hpp"
// #include "GL4_Effect.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Surface");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());
	Topl_Pipeline_GL4 pipeline;

	GL4_Effect_VertexShader vertexShader = GL4_Effect_VertexShader();
	GL4_Effect_FragmentShader fragmentShader = GL4_Effect_FragmentShader();
	renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);

	renderer.setCamera(&App::camera);
	renderer.buildScene(&App::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}