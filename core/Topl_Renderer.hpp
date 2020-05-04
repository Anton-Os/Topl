#include "native_os_def.h"
#include "Geometry.hpp"

class Topl_Renderer {
public:
    virtual void buildScene(const Topl_SceneGraph* sceneGraph) = 0;
    virtual void render(void) = 0;
	
	NATIVE_PLATFORM_ELEM m_native; // Native Platform Element required to create a renderer
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void cleanup(void) = 0;
    virtual void createPipeline(void) = 0;
};
