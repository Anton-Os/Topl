#include "Surface.hpp"

#include "Topl_Graphics_Drx11.hpp"
#include "Effect_Shader.hpp"
// #include "Drx11_Effect.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0], "Drx11 Surface");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow());

	Drx11_Effect_VertexShader vertexShader = Drx11_Effect_VertexShader(&platform);
	Drx11_Effect_FragmentShader fragmentShader = Drx11_Effect_FragmentShader();
	renderer.setPipeline(&vertexShader, &fragmentShader);

	renderer.setCamera(&Topl::camera);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}