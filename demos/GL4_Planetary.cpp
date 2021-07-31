#include "Planetary.hpp"

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
	
	Topl::scene.addGeometry("sphere", &Topl::sphereGeo);
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(PROJECTION_Ortho, SpatialBounds3D(3.0f));

	glPointSize(5.0f);
	glLineWidth(2.0f);
	Timer_Ticker gameTicker;
	
	Main::gameLoop(&platform, &renderer);

	return 0;
}