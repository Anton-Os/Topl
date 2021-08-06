#include "Canvas.hpp"

#include "Topl_Renderer_GL4.hpp"
#include "GL4_FrameFx.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	Main::init(&platform);

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_FrameFx.glsl";
	FrameFx_VertexShader vertexShader = FrameFx_VertexShader(&platform, vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_FrameFx.glsl";
	FrameFx_FragmentShader fragmentShader = FrameFx_FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);

	Topl::scene.addGeometry("plane", &Topl::planeGeo);
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(PROJECTION_Ortho, SpatialBounds3D(3.0f));

	glPointSize(5.0f);
	glLineWidth(3.0f);

	Main::gameLoop(&platform, &renderer);

	return 0;
}