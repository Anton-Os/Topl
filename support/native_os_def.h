#ifndef NATIVE_OS_DEF

#define TOPL_WIN_HEIGHT 1024
#define TOPL_WIN_WIDTH 1024

#include "support_def.h"

// OpenGL Specific Inclusions

#define GLEW_STATIC
#include "GL/glew.h"
#include <GL/gl.h>

// System Definitions

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <windowsx.h>
    // #include <ShellScalingApi.h>

    #include "resource.h"

    #define NATIVE_WINDOW HWND
    #define NATIVE_BITMAP BITMAP
    #define NATIVE_GL_CONTEXT HGLRC
    
    struct Windows_Platform_Context {
        WNDCLASS windowClass;
        NATIVE_WINDOW window;
        NATIVE_GL_CONTEXT oglCtx;
        HDC deviceCtx;
        POINT cursorPos;
        MSG eventMsg;
    };

    #define NATIVE_PLATFORM_CONTEXT Windows_Platform_Context

    #pragma comment(lib, "opengl32.lib")
#elif defined(__linux__)
    #include<X11/X.h>
    #include<X11/Xlib.h>

    #include<GL/glx.h> // checkout https://askubuntu.com/questions/306703/compile-opengl-program-missing-gl-gl-h

    #define NATIVE_WINDOW Window
    #define NATIVE_GL_CONTEXT GLXContext

    struct Cursor_Pos {
        int xPos;
        int yPos;
    };

    struct Linux_Platform_Context {
        Display* display;
        // XVisualInfo* visualInfo;
        NATIVE_WINDOW window;
        NATIVE_GL_CONTEXT oglCtx;
        Cursor_Pos cursorPos;
    };

    #define NATIVE_PLATFORM_CONTEXT Linux_Platform_Context
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
