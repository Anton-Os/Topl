#include "SimpleShapes.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "GL4_Volumes.hpp" // shader inclusion
#include "GL4_Flat.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Simple Shapes");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());

	Volumes_VertexShader vertexShader = Volumes_VertexShader();
	Flat_FragmentShader fragmentShader = Flat_FragmentShader();

	renderer.setCamera(&Topl::camera);
	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene, &Topl::camera);

	glPointSize(5.0f);
	glLineWidth(3.0f);

	Main::gameLoop(&platform, &renderer);

	return 0;
}