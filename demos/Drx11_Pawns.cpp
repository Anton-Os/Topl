#include "Pawns.hpp"

#include "Topl_Graphics_Drx11.hpp"
#include "Textured_Shader.hpp" // shader inclusion
// #include "Drx11_Textured.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "Drx11 Moving Sprite");
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getParentWindow()); // Renderer initialization
	Topl_Pipeline_Drx11 pipeline; // Pipeline declaration

	Drx11_Textured_VertexShader vertexShader = Drx11_Textured_VertexShader();
	Drx11_Textured_FragmentShader pixelShader = Drx11_Textured_FragmentShader();
	renderer.genPipeline(&pipeline, &vertexShader, &fragmentShader);
	
	renderer.setCamera(&Topl::camera);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}