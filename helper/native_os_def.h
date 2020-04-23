
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    // OpenGL support
    #include <GL/gl.h>
    #include <GL/glu.h>

    #define NATIVE_WINDOW HWND
    #define NATIVE_BITMAP BITMAP
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
    #include <DirectXMath.h>
#endif