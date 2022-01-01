#include "Surface.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "Effect_Shader.hpp"
// #include "GL4_Effect.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Surface");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());

	GL4_Effect_VertexShader vertexShader = GL4_Effect_VertexShader(&platform);
	GL4_Effect_FragmentShader fragmentShader = GL4_Effect_FragmentShader();
	renderer.setPipeline(&vertexShader, &fragmentShader);

	renderer.setCamera(&Topl::camera);
	renderer.buildScene(&Topl::scene);

	glPointSize(5.0f);
	glLineWidth(3.0f);

	Main::gameLoop(&platform, &renderer);

	return 0;
}