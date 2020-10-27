#include "native_os_def.h"

#include "FileIO.hpp"
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

	HWND wndWindow = CreateWindow(
		"Default Class",
		"Team Purple",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 1100,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(wndWindow, 1);
	UpdateWindow(wndWindow);

	MSG wndMessage;
	BOOL bRet;


    Topl_Renderer_GL4 renderer(wndWindow);

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_ColorWheel.glsl"; // Make unix fix
	Topl_Shader vertexShader(SHDR_Vertex, vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_ColorWheel.glsl"; // Make unix fix
	Topl_Shader fragmentShader(SHDR_Fragment, fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);

	Topl_SceneManager sMan1; // REMOVE LATER
	// Geo_RandShapes randShapes(&sMan1);
	Geo_CircleUp circleUp("prefix", &sMan1);

	renderer.buildScene(&sMan1);

	while ( renderer.renderScene(DRAW_Triangles)) {
		// Process input, this hould be the first update phase

		circleUp.updateSceneManager(&sMan1); // First object specific updates occur

		sMan1.resolvePhysics(); // Scene Manager needs to perform its internalized updates

		renderer.updateScene(&sMan1); // Renderer specific updating is the last thing to take place
	}

	return 0;
}