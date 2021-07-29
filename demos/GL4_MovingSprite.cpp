#include "MovingSprite.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "GL4_Basic.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Basic.glsl";
	Basic_VertexShader vertexShader = Basic_VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Basic.glsl";
	Basic_FragmentShader fragmentShader = Basic_FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene);

	Main::gameLoop(&platform, &renderer);

	return 0;
}