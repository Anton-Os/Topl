#include "GL4_SimpleShapes.hpp"

// Entry Point

int main(int argc, char** argv) {

<<<<<<< refs/remotes/origin/linux_port
#ifdef WIN32
	WNDCLASS wndClass = { 0 };
	// wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpfnWndProc = wndProc;
	wndClass.lpszClassName = "Default Class";
	RegisterClass(&wndClass);

	// Windows specific code block
	HWND wndWindow = CreateWindow(
		"Default Class",
		"Simple Shapes",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 1100,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(wndWindow, 1);
	UpdateWindow(wndWindow);

	// Main loop related variables
	MSG wndMessage;
	BOOL bRet;

	Topl_Renderer_GL4 renderer(wndWindow); // Renderer initialization
#elif defined(__linux__)
	Window lnxWindow;

	Topl_Renderer_GL4 renderer(lnxWindow);
#endif
=======
	Platform platform(argv[0]);
	platform.createWindow("Simple Shapes");
	// platform.setupMainWindow(mainWindow);
	Platform::keyLogger.addCallback('w', buttonCallback_w);
	Platform::keyLogger.addCallback('a', buttonCallback_a);
	Platform::keyLogger.addCallback('s', buttonCallback_s);
	Platform::keyLogger.addCallback('d', buttonCallback_d);
>>>>>>> local

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());

<<<<<<< refs/remotes/origin/linux_port
	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\VertexShader.glsl";
=======
	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.glsl";
>>>>>>> local
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\FragShader.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);

	renderer.buildScene(&Topl::sceneManager);

<<<<<<< refs/remotes/origin/linux_port
	while ( renderer.renderScene(DRAW_Triangles)) {
		// renderer.updateScene(&Topl::sceneManager);

=======
	glPointSize(5.0f); // TODO: Include as part of the renderer code
	while ( renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&Topl::sceneManager);
>>>>>>> local
		// Topl::sceneManager.resolvePhysics();

		platform.handleEvents();
	}

	return 0;
}
