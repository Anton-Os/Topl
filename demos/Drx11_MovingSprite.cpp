#include "Drx11_MovingSprite.hpp"

void callback_w(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(0.0f, MOVE_AMOUNT, 0.0f)); } // Move up
void callback_a(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(-1 * MOVE_AMOUNT, 0.0f, 0.0f)); } // Move left
void callback_s(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(0.0f, -1 * MOVE_AMOUNT, 0.0f)); } // Move down
void callback_d(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right

#ifdef WIN32
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	if (Topl::keyLogger.getCallbackCount() == 0) { // Initialization required
		Topl::keyLogger.addCallback('w', callback_w);
		Topl::keyLogger.addCallback('a', callback_a);
		Topl::keyLogger.addCallback('s', callback_s);
		Topl::keyLogger.addCallback('d', callback_d);
	}

	switch (message) {
	case (WM_CREATE): {}
	case (WM_PAINT): {}
	case(WM_KEYDOWN): {}
	case(WM_KEYUP): {}
	case (WM_CHAR): {
		Topl::keyLogger.addKeyPress((char)wParam);
	}
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

	WNDCLASS wndClass = { 0 };
	// wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpfnWndProc = wndProc;
	wndClass.lpszClassName = "Default Class";
	RegisterClass(&wndClass);

	// Windows specific code block
	HWND wndWindow = CreateWindow(
		"Default Class",
		"Moving Sprite",
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

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_MostBasic.hlsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string pixelShaderSrc = getParentDir(argv[0]) + "\\Pixel_MostBasic.hlsl";
	PixelShader pixelShader = PixelShader(pixelShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &pixelShader);

	renderer.buildScene(&Topl::sceneManager);

	while (renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&Topl::sceneManager);

		Topl::sceneManager.resolvePhysics();

		while (PeekMessage(&wndMessage, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wndMessage);
			DispatchMessage(&wndMessage);
		}
		if (wndMessage.message == WM_QUIT) break;
	}

	return 0;
}
