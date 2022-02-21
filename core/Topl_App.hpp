#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"
#ifdef _WIN32
#include "Topl_Renderer_Drx11.hpp"
#endif

#include "Topl_Scene.hpp"

enum APP_Backend {
    APP_OpenGL_4,
    APP_DirectX_11
    // Add Vulkan Support
};

struct Topl_App {
    Topl_App(APP_Backend backend) : _backend(backend) {
        // implement constructor body
    }

    virtual void init() = 0;
    virtual void loop() = 0;

	union Renderer {
		Topl_Renderer_GL4 GL4;
#ifdef _WIN32
		Topl_Renderer_Drx11 Drx11;
#endif
	};
    const enum APP_Backend _backend;
    
	Renderer* _renderer;
    Platform* _platform = nullptr;

    std::vector<Topl_Scene> _scenes;
};