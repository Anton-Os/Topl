#include "native_os_def.h"

#include "FileIO.hpp"
#include "Input.hpp"

#include "Topl_Renderer_GL4.hpp"

#include "composites/Geo_Construct.hpp"
#include "Chain.hpp"

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

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Flat.glsl";
	Topl_Shader vertexShader(SHDR_Vertex, vertexShaderSrc.c_str(), 
		{ Shader_Type("pos", SHDR_float_vec3) }
	);
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Flat.glsl";
	Topl_Shader fragmentShader(SHDR_Fragment, fragmentShaderSrc.c_str(), 
		{ Shader_Type("flatColor", SHDR_uint) }
	);

	renderer.setPipeline(&vertexShader, &fragmentShader);

	Topl_SceneManager sMan1;

	// Generic code block

	Geo_Sphere2D sphere = Geo_Sphere2D(0.2f, 12);
	Geo_Component component = Geo_Component((Geo_RenderObj*)&sphere);
	Geo_Chain_Properties chainProps = Geo_Chain_Properties(0.1f);
	Geo_Chain chain = Geo_Chain("chain1", &sMan1, &component, &chainProps, 8);

	renderer.buildScene(&sMan1);

	// Windows specific rendering and input processing
	MSG wndMessage;
	BOOL bRet;

	while ( renderer.renderScene(DRAW_Triangles)) {
		// renderer.updateScene(&sMan1);
		//sMan1.resolvePhysics();

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