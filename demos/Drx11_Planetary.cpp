#include "Planetary.hpp"

#include "Topl_Renderer_Drx11.hpp"
#include "Drx11_Volumes.hpp" // shader inclusion
#include "Drx11_Flat.hpp" // shader inclusion

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0]);
	Main::init(&platform);

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow()); // Renderer initialization

	Volumes_VertexShader vertexShader = Volumes_VertexShader();
	Flat_PixelShader fragmentShader = Flat_PixelShader();

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	Topl::scene.addGeometry("sphere", &Topl::sphereGeo);
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(PROJECTION_Ortho, SpatialBounds3D(3.0f));

	Timer_Ticker gameTicker;

	Main::gameLoop(&platform, &renderer);

	return 0;
}