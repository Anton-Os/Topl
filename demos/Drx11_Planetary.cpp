#include "Drx11_Planetary.hpp"

// Entry Point

int main(int argc, char** argv) {
	
	Platform platform(argv[0]);
	platform.createWindow("Planetary");
	Platform::keyLogger.addCallback('w', buttonCallback_w);
	Platform::keyLogger.addCallback('a', buttonCallback_a);
	Platform::keyLogger.addCallback('s', buttonCallback_s);
	Platform::keyLogger.addCallback('d', buttonCallback_d);

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow()); // Renderer initialization

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.hlsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Pixel_Flat.hlsl";
	PixelShader fragmentShader = PixelShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	Topl::scene.addGeometry("sphere", &Topl::sphere);
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(false, SpatialBounds3D(3.0f));

	Timer_Ticker gameTicker;

	Topl::sphere.updatePos(Eigen::Vector3f(0.0f, 0.0f, 2.0f));
	while (renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&Topl::scene);

		platform.handleEvents();
	}

	return 0;
}