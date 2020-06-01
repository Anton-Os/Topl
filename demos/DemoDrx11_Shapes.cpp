#include "native_os_def.h"
#include "Topl_Renderer_Drx11.hpp"

#include "Geometry/Geometry_Constructs.hpp"

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

    Topl_Renderer_Drx11 renderer(wndWindow);

	Topl_Shader vertexShader(SHDR_Vertex, "C:\\AntonDocs\\Codex\\Ao-Project\\Topl-Skeleton\\MSVC_BUILD_2\\Debug\\Vertex_MostBasic.hlsl");
	Topl_Shader fragmentShader(SHDR_Fragment, "C:\\AntonDocs\\Codex\\Ao-Project\\Topl-Skeleton\\MSVC_BUILD_2\\Debug\\Pixel_MostBasic.hlsl");

	renderer.createPipeline(&vertexShader, &fragmentShader);

	Topl_SceneGraph sGraph1;
	Geo_RandShapes randShapes(&sGraph1);

	renderer.buildScene(&sGraph1);

    for(unsigned t = 0; t < 99999; t++)
        renderer.renderScene(DRAW_Triangles);

	return 0;
}