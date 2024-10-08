#ifndef NATIVE_OS_DEF

#define TOPL_WIN_HEIGHT 1024
#define TOPL_WIN_WIDTH 1024

#include "support_def.h"

// System Definitions

#define GLEW_STATIC
#include "GL/glew.h"
#include <GL/gl.h>

#ifdef __linux__
#include<GL/glx.h> // checkout https://askubuntu.com/questions/306703/compile-opengl-program-missing-gl-gl-h
#endif

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <windowsx.h>
    #include <shellapi.h>
    #include <shobjidl.h>
    #include <Ole2.h>

    #include <io.h> // for files
    #define F_OK 0
    #define access _access

    #include "resource.h"

    #define NATIVE_WINDOW HWND
    #define NATIVE_BITMAP BITMAP
    #define NATIVE_GL_CONTEXT HGLRC
    
    struct Windows_Platform_Context {
        WNDCLASS windowClass;
        NATIVE_WINDOW window;
        NATIVE_GL_CONTEXT oglCtx;
        // IDropTarget dropTarget;
        HDC deviceCtx;
        POINT cursorPos;
        MSG eventMsg;
    };

    #define NATIVE_PLATFORM_CONTEXT Windows_Platform_Context

    #pragma comment(lib, "opengl32.lib")
#elif defined(__linux__)
    #include<X11/X.h>
    #include<X11/Xlib.h>

    #include <dlfcn.h>

    #define NATIVE_WINDOW Window
    #define NATIVE_GL_CONTEXT GLXContext

    struct Cursor_Pos {
        int xPos;
        int yPos;
    };

    struct Linux_Platform_Context {
        Display* display;
        XVisualInfo* visualInfo;
        NATIVE_WINDOW window;
        NATIVE_GL_CONTEXT oglCtx;
        Cursor_Pos cursorPos;
    };

    #define NATIVE_PLATFORM_CONTEXT Linux_Platform_Context
    // #elif defined(__ANDROID__) // TODO: Check Android Implementation
#else // No Support
    typedef struct Dummy { };

    #define NATIVE_WINDOW Dummy
    #define NATIVE_GL_CONTEXT Dummy

    struct Dummy_Platform_Context { // placeholder value
        NATIVE_WINDOW window;
        NATIVE_GL_CONTEXT oglCtx;
    };

    #define NATIVE_PLATFORM_CONTEXT Dummy_Platform_Context
#endif

#define NATIVE_OS_DEF
#endif
