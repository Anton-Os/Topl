#include "SimpleShapes.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "GL4_Volumes.hpp" // shader inclusion
#include "GL4_Flat.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.glsl";
	Volumes_VertexShader vertexShader = Volumes_VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Flat.glsl";
	Flat_FragmentShader fragmentShader = Flat_FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(false, SpatialBounds3D(3.0f));

	glPointSize(5.0f);
	glLineWidth(3.0f);

	Main::gameLoop(&platform, &renderer);

	return 0;
}