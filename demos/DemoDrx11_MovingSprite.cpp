#include "native_os_def.h"

#include "FileIO.hpp"

#include "Topl_Renderer_Drx11.hpp"

#include "composites/Geo_Construct.hpp"
#include "Humanoid.hpp"

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

// Callback function specific to Vertex_MostBasic.hlsl
void vertexBlockCallback(const Geo_Component* component, std::vector<uint8_t>* bytes) {
	bytes->resize(sizeof(float) * 8); // Size should be known
	const unsigned short posOffset = 0;
	const unsigned short angleOffset = 4;

	// TODO: Fix this!!!
	// bytes->at(posOffset) = component->getPos; // Set first 4 floats to contents of component position
	// bytes->at(angleOffset) = component->getAngles; // Set remaining 4 floats to contents of component angles
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


    Topl_Renderer_Drx11 renderer(wndWindow);

	/* std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_MostBasic.hlsl";
	Topl_Shader vertexShader(SHDR_Vertex, vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Pixel_MostBasic.hlsl";
	Topl_Shader fragmentShader(SHDR_Fragment, fragmentShaderSrc.c_str()); */

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_MostBasic.hlsl";
	Topl_Shader vertexShader(SHDR_Vertex, vertexShaderSrc.c_str(), { 
		Shader_Input("pos", "POSITION", SHDR_float_vec3), Shader_Input("texcoord", "TEXCOORD", SHDR_float_vec2) 
	}, vertexBlockCallback);
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Pixel_MostBasic.hlsl";
	Topl_Shader fragmentShader(SHDR_Fragment, fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);

	Topl_SceneManager sMan1;

	// TODO: Make these not device specific, relative file paths only!
	std::pair<const char*, Eigen::Vector3f> humanoidProps[ANATOMY_PARTS_COUNT] = {
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Head.png", Eigen::Vector3f(0.0f, 0.11f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftArm.png", Eigen::Vector3f(0.0f, -0.1f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightArm.png", Eigen::Vector3f(0.12f, -0.14f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Body.png", Eigen::Vector3f(-0.12f, -0.14f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftLeg.png", Eigen::Vector3f(0.06f, -0.35f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightLeg.png", Eigen::Vector3f(-0.06f, -0.35f, 0.0))
	};

	// Geo_Humanoid humanoid("humanoid", &sMan1);
	Geo_Humanoid humanoid("humanoid", &sMan1, humanoidProps, 0.25f);
	humanoid.move(&sMan1, Eigen::Vector3f(0.5f, 0.5f, 0.0f)); // Moving humanoid
	// humanoid.rotate(&sMan1, Eigen::Vector3f(4.0f, 4.0f, 0.0f));

	renderer.buildScene(&sMan1);

	MSG wndMessage;
	BOOL bRet;

	while (renderer.renderScene(DRAW_Triangles)) {
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