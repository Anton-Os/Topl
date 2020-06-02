#include "native_os_def.h"
#include "Topl_Renderer_Drx11.hpp"

//#pragma comment(lib, "D3d12.lib")
//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "dxgi.lib")

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

	Geo_Rect2D gRect1(1.0f, 1.0f);

	Topl_GeoNode gEntity1((Geo_RenderObj*)&gRect1);

	Topl_SceneGraph sGraph1; // REMOVE LATER
	sGraph1.addGeometry("box", &gEntity1);
	renderer.buildScene(&sGraph1);

	while (renderer.renderScene(DRAW_Triangles)) {
		// Process input and other things, on successful rendering
	}

	return 0;
}