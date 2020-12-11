#include "native_os_def.h"

#include "FileIO.hpp"
#include "Input.hpp"
#include "Geo_Constructs.hpp"

#include "Topl_Renderer_GL4.hpp"

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
	}
	case (WM_PAINT): {
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

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
		"Team Purple",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 1100,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(wndWindow, 1);
	UpdateWindow(wndWindow);

	// OpenGL Specific code block
    Topl_Renderer_GL4 renderer(wndWindow);

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\VertexShader.glsl"; // Make unix fix
	Topl_Shader vertexShader(SHDR_Vertex, vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\FragShader.glsl"; // Make unix fix
	Topl_Shader fragmentShader(SHDR_Fragment, fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);

	Topl_SceneManager sMan1;

	// Generic code block
	// Geo_Humanoid humanoid1("humanoid1", &sMan1);
	Geo_Humanoid humanoid2("humanoid2", &sMan1);
	humanoid2.move(&sMan1, Eigen::Vector3f(0.9f, 0.3, 0.0)); // Moving humanoid

	renderer.buildScene(&sMan1);

	// Windows specific rendering and input processing
	MSG wndMessage;
	BOOL bRet;

	while ( renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&sMan1);

		sMan1.resolvePhysics();

		// Input processing, check if it works unhinged
		while (PeekMessage(&wndMessage, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&wndMessage);
			DispatchMessage(&wndMessage);
		}

		if (wndMessage.message == WM_QUIT) break;
	}

	return 0;
}