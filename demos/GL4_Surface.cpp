#include "Surface.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "GL4_Effect.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Surface");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());

	Effect_VertexShader vertexShader = Effect_VertexShader(&platform);
	Effect_FragmentShader fragmentShader = Effect_FragmentShader();

	renderer.setCamera(&Topl::camera);
	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene, &Topl::camera);

	glPointSize(5.0f);
	glLineWidth(3.0f);

	Main::gameLoop(&platform, &renderer);

	return 0;
}