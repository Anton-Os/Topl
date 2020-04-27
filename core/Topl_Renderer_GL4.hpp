#include "Topl_Renderer.hpp"

class Topl_Renderer_GL4 : Topl_Renderer {
public:
    Topl_Renderer_GL4(HWND hwnd){ init(hwnd); }
    ~Topl_Renderer_GL4(){ cleanup(); }

    // void buildScene(const Topl_SceneGraph* sceneGraph);
    void buildScene(void);
    void render(void);
private:
    void init(NATIVE_WINDOW hwnd);
    void createPipeline(void);
    void cleanup(void);

    //NATIVE_GL_CONEXT m_GL4_Ctx;
};