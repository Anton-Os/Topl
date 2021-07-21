#include "GL4_MovingSprite.hpp"

// Entry Point

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	platform.createWindow("Moving Sprite");
	// platform.setupMainWindow(mainWindow);
	Platform::keyLogger.addCallback('w', buttonCallback_w);
	Platform::keyLogger.addCallback('a', buttonCallback_a);
	Platform::keyLogger.addCallback('s', buttonCallback_s);
	Platform::keyLogger.addCallback('d', buttonCallback_d);
	Platform::keyLogger.addCallback('r', buttonCallback_r);

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Basic.glsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Basic.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl::scene);

	while ( renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&Topl::scene);
		Topl::scene.resolvePhysics();

		platform.handleEvents();
	}

	return 0;
}