#include "MovingSprite.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "Textured_Shader.hpp" // shader inclusion
// #include "GL4_Textured.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0], "GL4 Moving Sprite");
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getParentWindow());

	GL4_Textured_VertexShader vertexShader = GL4_Textured_VertexShader();
	GL4_Textured_FragmentShader fragmentShader = GL4_Textured_FragmentShader();
	// Textured_VertexShader vertexShader = Textured_VertexShader();
	// Textured_FragmentShader fragmentShader = Textured_FragmentShader();

	renderer.setCamera(&Topl::camera);
	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene, &Topl::camera);

	Main::gameLoop(&platform, &renderer);

	return 0;
}