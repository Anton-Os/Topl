#ifndef NATIVE_OS_DEF

#define WIN_HEIGHT 500
#define WIN_WIDTH 500

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    // OpenGL support
    #include <GL/gl.h>
    #include <GL/glu.h>

    #define NATIVE_WINDOW HWND
    #define NATIVE_BITMAP BITMAP
    #define NATIVE_GL_CONTEXT HGLRC

	struct _Native_Platform_Elem_win {
		NATIVE_WINDOW* window; // Window is initialized outside
		NATIVE_GL_CONTEXT GL_Ctx;
		HDC windowDevice_Ctx;
	};

	#define NATIVE_PLATFORM_ELEM _Native_Platform_Elem_win

    #pragma comment(lib, "opengl32.lib")
#endif

#if defined (_WIN32) && defined (_MSC_VER)

    // Windows runtime library COM-Specific items
    // #include <wrl.h>
    // using namespace Microsoft::WRL;

    // #include "d3dx12.h" // Helper that also includes d3d12.h
    
    #include <d3d11.h>
    #include "d3dx10.h"
    #include "d3dx11.h"

    #include <dxgi1_4.h>
    #include <d3dcompiler.h>
    //#include <xnamath.h>
    #include <DirectXMath.h>

    #pragma comment(lib, "d3d11.lib")
    //#pragma comment(lib, "d3dx11.lib")
    //#pragma comment(lib, "d3dx10.lib")
#endif

#define NATIVE_OS_DEF
#endif