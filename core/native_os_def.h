#ifndef NATIVE_OS_DEF

#define WIN_HEIGHT 500
#define WIN_WIDTH 500

<<<<<<< refs/remotes/origin/linux_port
#include "Rasteron.h" // TODO: Make this inclusion conditional

#define GLEW_STATIC
//#define GLEWAPI extern
#include "GL/glew.h"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
	  #include <windowsx.h>
=======
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <windowsx.h>
>>>>>>> local

    // OpenGL support
    /* #include <GL/gl.h>
    #include <GL/glu.h> */

    #define NATIVE_WINDOW HWND
    #define NATIVE_BITMAP BITMAP
    #define NATIVE_GL_CONTEXT HGLRC

<<<<<<< refs/remotes/origin/linux_port
	struct _Native_Platform_Elem_win {
		NATIVE_WINDOW* window; // Window is initialized outside
		NATIVE_GL_CONTEXT GL_Ctx;
		HDC windowDevice_Ctx;
	};
=======
    struct Windows_Platform_Context {
        WNDCLASS windowClass;
        NATIVE_WINDOW window; // Window is initialized outside // TODO: Fix this!
        NATIVE_GL_CONTEXT GL_ctx;
        HDC windowDevice_Ctx;
        POINT cursorPos;
        MSG eventMsg;
    };
>>>>>>> local

	#define NATIVE_PLATFORM_ELEM _Native_Platform_Elem_win

  #pragma comment(lib, "opengl32.lib")
#elif defined(__linux__)
  #include<X11/X.h>
  #include<X11/Xlib.h>

  #undef Success // Potential Eigen fix

  // OpenGL Support
  #include<GL/gl.h>
  #include<GL/glx.h>

  #define NATIVE_WINDOW Window
  #define NATIVE_GL_CONTEXT GLXContext

  struct Native_Cursor_Pos {
    int xPos;
    int yPos;
  };

<<<<<<< refs/remotes/origin/linux_port
  #define NATIVE_CURSOR_POS Native_Cursor_Pos // X and Y coordinates of the cursor
=======
    struct Linux_Platform_Context {
        Display* display;
        // XVisualInfo* visualInfo;
        NATIVE_WINDOW window;
        NATIVE_GL_CONTEXT GL_ctx;
        Cursor_Pos cursorPos;
    };
>>>>>>> local

  struct _Native_Platform_Context {
    Display* display;
    NATIVE_WINDOW* window;
    NATIVE_GL_CONTEXT GL_Ctx;
  };
#endif

#if defined (_WIN32) && defined (_MSC_VER)

    #include <d3d11_1.h>
	#include <dxgi.h> // additional
    #include <wrl/client.h> // additional

    #include <dxgi1_4.h>
    #include <d3dcompiler.h>
    // #include <xnamath.h>

    #pragma comment(lib, "d3d11.lib")
    #pragma comment(lib, "d3dcompiler.lib")
    // #pragma comment(lib, "d3dx11.lib")
    // #pragma comment(lib, "d3dx10.lib")
#endif

#define NATIVE_OS_DEF
#endif
