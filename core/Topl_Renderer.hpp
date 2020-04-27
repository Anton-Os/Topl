#include "native_os_def.h"
#include "Geometry.hpp"

// Virutal class
/* class Topl_Renderer {
public:
    virtual Topl_Renderer* buildScene(Topl_SceneGraph sceneGraph);
private:
    virtual Topl_Renderer* init(NATIVE_WINDOW hwnd);
    virtual Topl_Renderer* render(void);
}; */

class Topl_Renderer {
public:
    // virtual void buildScene(const Topl_SceneGraph* sceneGraph) = 0;
    virtual void buildScene(void) = 0;
    virtual void render(void) = 0;
	
	NATIVE_PLATFORM_ELEM m_native; // Native Platform Element required to create a renderer
	// NATIVE_WINDOW* m_window; // Renderer is associated to a window
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void cleanup(void) = 0;
    virtual void createPipeline(void) = 0;
};
