#include "GL4_SimpleShapes.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	platform.createWindow("Simple Shapes");
	// platform.setupMainWindow(mainWindow);
	Platform::keyLogger.addCallback('w', buttonCallback_w);
	Platform::keyLogger.addCallback('a', buttonCallback_a);
	Platform::keyLogger.addCallback('s', buttonCallback_s);
	Platform::keyLogger.addCallback('d', buttonCallback_d);

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.glsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Flat.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(false, SpatialBounds3D(3.0f));

	glPointSize(5.0f);
	glLineWidth(3.0f);
	Timer_Ticker gameTicker;

	while (renderer.renderScene(DRAW_Triangles)) {
		Topl::chain.rotate(&Topl::scene, Eigen::Vector2f(-0.0001 * gameTicker.getAbsSecs(), 0.0));
		Topl::grid.rotate(&Topl::scene, Eigen::Vector2f(0.0, 0.0001 * gameTicker.getAbsSecs()));
		renderer.updateScene(&Topl::scene);
		// Topl::scene.resolvePhysics();

		platform.handleEvents();
	}

	return 0;
}