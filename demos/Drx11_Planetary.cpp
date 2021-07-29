#include "Planetary.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_Volumes.hpp" // shader inclusion
#include "Drx11_Flat.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0]);
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow()); // Renderer initialization

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.hlsl";
	Volumes_VertexShader vertexShader = Volumes_VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Pixel_Flat.hlsl";
	Flat_PixelShader fragmentShader = Flat_PixelShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	Topl::scene.addGeometry("sphere", &Topl::sphere);
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(false, SpatialBounds3D(3.0f));

	Timer_Ticker gameTicker;

	Main::gameLoop(&platform, &renderer);

	return 0;
}