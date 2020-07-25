#ifndef NATIVE_OS_DEF

#define WIN_HEIGHT 500
#define WIN_WIDTH 500

#include "Rasteron.h" // TODO: Make this inclusion conditional

#define GLEW_STATIC
//#define GLEWAPI extern
#include "GL/glew.h"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
	#include <windowsx.h>

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
    
    #include <d3d11_1.h>
	#include <dxgi.h> // additional
    #include <wrl/client.h> // additional
    #include "d3dx10.h"
    #include "d3dx11.h"

    #include <dxgi1_4.h>
    #include <d3dcompiler.h>
    // #include <xnamath.h>
	#include <d3dx10math.h>

    #pragma comment(lib, "d3d11.lib")
    #pragma comment(lib, "d3dcompiler.lib")
    // #pragma comment(lib, "d3dx11.lib")
    // #pragma comment(lib, "d3dx10.lib")
#endif

#define NATIVE_OS_DEF
#endif