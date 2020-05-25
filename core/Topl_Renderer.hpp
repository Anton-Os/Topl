#include "native_os_def.h"

#include "Topl_Shader.hpp"
#include "Topl_SceneGraph.hpp"

class Topl_Renderer {
public:
	virtual ~Topl_Renderer() {};

    void createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragmentShader){
        mVertexShader = vertexShader;
        mFragmentShader = fragmentShader;
        createPipeline(); // Call the virtual function
    }
    virtual void buildScene(const Topl_SceneGraph* sceneGraph) = 0;
    virtual void render(void) = 0;
	
	NATIVE_PLATFORM_ELEM m_native; // Native Platform Element required to create a renderer
protected:
    bool mPipelineReady = false; // Set to true
    bool mSceneReady = false;

    const Topl_Shader* mVertexShader; // May need an instance instead
    const Topl_Shader* mFragmentShader; // May need an instance instead
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void createPipeline(void) = 0; // 
};
