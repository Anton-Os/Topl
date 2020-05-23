#include "Topl_Renderer.hpp"

class Topl_Renderer_GL4 : Topl_Renderer {
public:
    Topl_Renderer_GL4(HWND hwnd){ init(hwnd); }
	~Topl_Renderer_GL4();

    // void buildScene(const Topl_SceneGraph* sceneGraph);
    void buildScene(const Topl_SceneGraph* sceneGraph) override;
    void render(void) override;
private:
    void init(NATIVE_WINDOW hwnd) override;
    void createPipeline(void) override;
    // void cleanup(void) override;

	GLuint vertexBuff; // One buffer for now
	GLuint vertexArray;
	GLuint vShader; // Vertex Shader
	GLuint fShader; // Fragment Shader
	GLuint shaderProg;

    //NATIVE_GL_CONEXT m_GL4_Ctx;
};