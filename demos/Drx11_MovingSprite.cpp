#include "native_os_def.h"

#include "FileIO.hpp"
#include "Input.hpp"

#include "Topl_Renderer_Drx11.hpp"

#include "Geo_Construct.hpp"
#include "composites/Humanoid.hpp"

#define MOVE_AMOUNT 8.0

namespace Topl_Demo {
	Topl_SceneManager sceneManager;

	// TODO: Make these not device specific, relative file paths only!
	std::pair<const char*, Eigen::Vector3f> humanoidProps[HUMANOID_PARTS_COUNT] = {
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Head.png", Eigen::Vector3f(0.0f, 0.11f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftArm.png", Eigen::Vector3f(0.0f, -0.1f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightArm.png", Eigen::Vector3f(0.12f, -0.14f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Body.png", Eigen::Vector3f(-0.12f, -0.14f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftLeg.png", Eigen::Vector3f(0.06f, -0.35f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightLeg.png", Eigen::Vector3f(-0.06f, -0.35f, 0.0))
	};

	// Geo_Humanoid humanoid;
	Geo_Humanoid humanoid("humanoid", &sceneManager, humanoidProps, 0.25f);
}

KeyState keyState_w('w', KEY_release);
void callback_w(void) { Topl_Demo::humanoid.move(&Topl_Demo::sceneManager, Eigen::Vector3f(0.0f, MOVE_AMOUNT, 0.0f)); } // Move up
KeyState keyState_a('a', KEY_release);
void callback_a(void) { Topl_Demo::humanoid.move(&Topl_Demo::sceneManager, Eigen::Vector3f(-1 * MOVE_AMOUNT, 0.0f, 0.0f)); } // Move left
KeyState keyState_s('s', KEY_release);
void callback_s(void) { Topl_Demo::humanoid.move(&Topl_Demo::sceneManager, Eigen::Vector3f(0.0f, -1 * MOVE_AMOUNT, 0.0f)); } // Move down
KeyState keyState_d('d', KEY_release);
void callback_d(void) { Topl_Demo::humanoid.move(&Topl_Demo::sceneManager, Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right

static enum KEY_Event currentEvent = KEY_none; // Move to a separate destination!

#ifdef WIN32
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	static Input_KeyLogger keyLogger;
	if (keyLogger.getCallbackCount() == 0) { // Initialization required
		keyLogger.addCallback(&keyState_w, callback_w);
		keyLogger.addCallback(&keyState_a, callback_a);
		keyLogger.addCallback(&keyState_s, callback_s);
		keyLogger.addCallback(&keyState_d, callback_d);
	}

	switch (message) {
	case (WM_CREATE): {}
	case (WM_PAINT): {}
	case(WM_KEYDOWN): {
		currentEvent = KEY_press;
	}
	case(WM_KEYUP): {
		currentEvent = KEY_release;
	}
	case (WM_CHAR): {
		if (currentEvent != KEY_none) {
			keyLogger.addKeyEvent((char)wParam, currentEvent);
			currentEvent = KEY_none; // Reset the event to prevent extra triggers
		}
	}
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
			{ Shader_Type("pos", "POSITION", SHDR_float_vec3), Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) } // Inputs
		) {  }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		const uint8_t* offsetBytesPtr = reinterpret_cast<const uint8_t*>(component->getPos()->data());
		const uint8_t* rotationBytesPtr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());

		bytes->assign({
			*(offsetBytesPtr), *(offsetBytesPtr + 1), *(offsetBytesPtr + 2), 0, // Gets offset values for x, y, and z, 0 for padding
			*(rotationBytesPtr), *(rotationBytesPtr + 1), 0, 0 // Gets rotations values on x and y axis, 0's for padding
		});

		bytes->assign({
			*(offsetBytesPtr + 0), *(offsetBytesPtr + 1), *(offsetBytesPtr + 2), *(offsetBytesPtr + 3), // X offset value
			*(offsetBytesPtr + 4), *(offsetBytesPtr + 5), *(offsetBytesPtr + 6), *(offsetBytesPtr + 7), // Y offset value
			*(offsetBytesPtr + 8), *(offsetBytesPtr + 9), *(offsetBytesPtr + 10), *(offsetBytesPtr + 11), // Z offset value
			0, 0, 0, 0, // 0 byte padding
			*(rotationBytesPtr + 0), *(rotationBytesPtr + 1), *(rotationBytesPtr + 2), *(rotationBytesPtr + 3), // Main axis rotation
			*(rotationBytesPtr + 4), *(rotationBytesPtr + 5), *(rotationBytesPtr + 6), *(rotationBytesPtr + 7), // Cross axis rotation
			0, 0, 0, 0, // 0 byte padding
			0, 0, 0, 0 // 0 byte padding
		});

		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		return false; // No implementation
	}
};

struct PixelShader : public Topl_Shader {
	PixelShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("pos", "POSITION", SHDR_float_vec3), Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		return false; // No implementation
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		return false; // No implementation
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
#else
	// TODO: make comparable UNIX version
#endif

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_MostBasic.hlsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string pixelShaderSrc = getParentDir(argv[0]) + "\\Pixel_MostBasic.hlsl";
	PixelShader pixelShader = PixelShader(pixelShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &pixelShader);

	renderer.buildScene(&Topl_Demo::sceneManager);

	while (renderer.renderScene(DRAW_Triangles)) {
		renderer.updateScene(&Topl_Demo::sceneManager);

		Topl_Demo::sceneManager.resolvePhysics();
		
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