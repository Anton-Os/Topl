#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"
#ifdef _WIN32
#include "Topl_Renderer_Drx11.hpp"
#endif

enum APP_Backend {
    APP_OpenGL_4,
    APP_DirectX_11
    // Add Vulkan Support
};

struct Topl_App {
    Topl_App(APP_Backend b) : backend(b) {
        // implement constructor body
    }

    virtual void init() = 0;
    virtual void loop() = 0;

    Platform* platform;
    union Renderer {
        Topl_Renderer_GL4 GL4
#ifdef _WIN32
        Topl_Renderer_Drx11 Drx11
#endif
    } *renderer;
    const enum APP_Backend backend;
};