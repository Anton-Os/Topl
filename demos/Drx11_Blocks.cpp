#include "SimpleShapes.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Flat_Shader.hpp"

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Simple Shapes");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization

	Drx11_Flat_VertexShader vertexShader = Drx11_Flat_VertexShader();
	Drx11_Flat_FragmentShader fragmentShader = Drx11_Flat_FragmentShader();

	renderer.setCamera(&Topl::camera);
	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}