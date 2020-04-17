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
    virtual void buildScene(Topl_SceneGraph sceneGraph);
    virtual void render(void);
private:
    virtual void init(NATIVE_WINDOW hwnd);
};

