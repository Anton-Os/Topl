#include "GL4_SimpleShapes.hpp"

#ifdef WIN32
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {}
	case (WM_PAINT): {}
	case(WM_KEYDOWN): {}
	case(WM_KEYUP): {}
	case (WM_CHAR): {}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
#else
	// TODO: make comparable UNIX version
#endif

// Entry Point

int main(int argc, char** argv) {

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

	std::string parentDir = getParentDir(argv[0]);

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\VertexShader.glsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\FragShader.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);

	renderer.buildScene(&Topl::sceneManager);

	while ( renderer.renderScene(DRAW_Triangles)) {
		// renderer.updateScene(&Topl::sceneManager);

		// Topl::sceneManager.resolvePhysics();

#ifdef WIN32
		while (PeekMessage(&wndMessage, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wndMessage);
			DispatchMessage(&wndMessage);
		}
		if (wndMessage.message == WM_QUIT) break;
#else
		// TODO: make comparable UNIX version
#endif
	}

	return 0;
}
