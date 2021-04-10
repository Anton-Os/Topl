#include "Drx11_SimpleShapes.hpp"

// Entry Point

int main(int argc, char** argv) {
<<<<<<< refs/remotes/origin/linux_port

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

	Topl_Renderer_Drx11 renderer(wndWindow); // Renderer initialization
=======
	
	Platform platform(argv[0]);
	platform.createWindow("Simple Shapes");
	//platform.setupMainWindow(mainWindow);
	Platform::keyLogger.addCallback('w', buttonCallback_w);
	Platform::keyLogger.addCallback('a', buttonCallback_a);
	Platform::keyLogger.addCallback('s', buttonCallback_s);
	Platform::keyLogger.addCallback('d', buttonCallback_d);
>>>>>>> local

	Topl_Renderer_Drx11 renderer(platform.getNativeWindow()); // Renderer initialization

<<<<<<< refs/remotes/origin/linux_port
	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_MostBasic.hlsl";
=======
	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.hlsl";
>>>>>>> local
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Pixel_MostBasic.hlsl";
	PixelShader fragmentShader = PixelShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);

	renderer.buildScene(&Topl::sceneManager);

	while ( renderer.renderScene(DRAW_Triangles)) {
<<<<<<< refs/remotes/origin/linux_port
		// renderer.updateScene(&Topl::sceneManager);

=======
		renderer.updateScene(&Topl::sceneManager);
>>>>>>> local
		// Topl::sceneManager.resolvePhysics();

		platform.handleEvents();
	}

	return 0;
}
