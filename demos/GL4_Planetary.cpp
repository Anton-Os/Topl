#include "GL4_Planetary.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	platform.createWindow("Planetary");
	Platform::keyLogger.addCallback('w', buttonCallback_w);
	Platform::keyLogger.addCallback('a', buttonCallback_a);
	Platform::keyLogger.addCallback('s', buttonCallback_s);
	Platform::keyLogger.addCallback('d', buttonCallback_d);

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.glsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Swirls.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	Topl::scene.addGeometry("sphere", &Topl::sphere);
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(false, SpatialBounds3D(3.0f));

	glPointSize(5.0f);
	glLineWidth(2.0f);
	Timer_Ticker gameTicker;
	
	// Topl::sphere.updatePos(Eigen::Vector3f(0.0f, 0.0f, -1.0f));
	while (renderer.renderScene(DRAW_Lines)) {
		renderer.updateScene(&Topl::scene);

		platform.handleEvents();
	}

	return 0;
}