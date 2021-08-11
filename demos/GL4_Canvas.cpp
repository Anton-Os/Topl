#include "Canvas.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "GL4_FrameFx.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());

	FrameFx_VertexShader vertexShader = FrameFx_VertexShader(&platform);
	FrameFx_FragmentShader fragmentShader = FrameFx_FragmentShader();

	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene);

	glPointSize(5.0f);
	glLineWidth(3.0f);

	Main::gameLoop(&platform, &renderer);

	return 0;
}