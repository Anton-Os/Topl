#include "native_os_def.h"

#include "FileIO.hpp"
#include "Topl_Renderer_GL4.hpp"

#include "primitives/Geo_Sphere2D.hpp"
#include "primitives/Geo_Rect3D.hpp"
#include "Geo_Construct.hpp"
#include "composites/Grid.hpp"
#include "composites/Chain.hpp"

#define MOVE_AMOUNT 8.0

namespace Topl_Demo {
	Topl_SceneManager sceneManager;

	Geo_Rect3D rect1 = Geo_Rect3D(0.3f, 0.4f, 0.2f);
	Geo_Component gridGeo = Geo_Component((const Geo_RenderObj*)&rect1);
	Geo_Grid_Properties gridProps = Geo_Grid_Properties(
		std::make_pair(3, 0.1f), // Width count and distance apart
		std::make_pair(3, 0.1f), // Height count and distance apart
		std::make_pair(4, 0.2f) // Depth count and distance apart
	);
	Geo_Grid grid("grid", &sceneManager, &gridGeo, &gridProps);

	Geo_Sphere2D sphere1 = Geo_Sphere2D(0.1f, 30);
	Geo_Component chainGeo = Geo_Component((const Geo_RenderObj*)&sphere1);
	Geo_Chain_Properties chainProps = Geo_Chain_Properties(0.3f); // 0.1f is the distance apart
	Geo_Chain chain("chain", &sceneManager, &chainGeo, &chainProps, 4);
}

#ifdef WIN32
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {}
	case (WM_PAINT): {}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
#else
	// TODO: make comparable UNIX version
#endif

// TODO: Move this into .hpp definition

struct VertexShader : public Topl_Shader {
	VertexShader(const char* filePath)
		: Topl_Shader(
			SHDR_Vertex, filePath,
			{ Shader_Type("pos", SHDR_float_vec3), Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		const uint8_t* rotationBytesPtr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
		const uint8_t* offsetBytesPtr = reinterpret_cast<const uint8_t*>(component->getPos()->data());

		bytes->assign({ 
			*(rotationBytesPtr + 0), *(rotationBytesPtr + 1), *(rotationBytesPtr + 2), *(rotationBytesPtr + 3), // Main axis rotation
			*(rotationBytesPtr + 4), *(rotationBytesPtr + 5), *(rotationBytesPtr + 6), *(rotationBytesPtr + 7), // Cross axis rotation
			0, 0, 0, 0, // 0 byte padding
			0, 0, 0, 0, // 0 byte padding
			*(offsetBytesPtr + 0), *(offsetBytesPtr + 1), *(offsetBytesPtr + 2), *(offsetBytesPtr + 3), // X offset value
			*(offsetBytesPtr + 4), *(offsetBytesPtr + 5), *(offsetBytesPtr + 6), *(offsetBytesPtr + 7), // Y offset value
			*(offsetBytesPtr + 8), *(offsetBytesPtr + 9), *(offsetBytesPtr + 10), *(offsetBytesPtr + 11), // Z offset value
			0, 0, 0, 0 // 0 byte padding
		});

		return true; // Indicates that an implementation exists
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		return false;
	}
};

struct FragmentShader : public Topl_Shader {
	FragmentShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		return false; // Indicates that an implementation is absent
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		return false;
	}                                                                       
};

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
		"Shapes",
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
#else
	// TODO: make comparable UNIX version
#endif

	std::string parentDir = getParentDir(argv[0]);

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\VertexShader.glsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\FragShader.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl_Demo::sceneManager);

	while ( renderer.renderScene(DRAW_Triangles)) {
		// renderer.updateScene(&Topl_Demo::sceneManager);

		// Topl_Demo::sceneManager.resolvePhysics();

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