#include "Topl_Renderer.hpp"

struct Buffer_GL4 {
	unsigned targetID;
	enum BUFF_Type type;
	GLuint buffer;
	unsigned count = 1;
};

#define GL4_BUFFER_MAX 1024

class Topl_BufferAlloc_GL4 {
public:
	Topl_BufferAlloc_GL4(){}

	GLuint getAvailableBuff();
// private:
public:
	bool mIsInit = false;
	void init() { glGenBuffers(GL4_BUFFER_MAX, &slots[0]); mIsInit = true; }

	GLuint slots[GL4_BUFFER_MAX];
	unsigned slotIndex = 0; // May need to be zero
};

#define GL4_VERTEX_ARRAY_CAPACITY 1024

struct Topl_Pipeline_GL4 {
	GLuint vertexDataLayouts[GL4_VERTEX_ARRAY_CAPACITY];
	unsigned layoutIndex = 0;

	GLuint vShader; // Vertex Shader // Make to vector
	GLuint fShader; // Fragment Shader // Make to vector
	GLuint shaderProg;
};

class Topl_Renderer_GL4 : public Topl_Renderer {
public:
    Topl_Renderer_GL4(HWND hwnd){ init(hwnd); }
	~Topl_Renderer_GL4();

    void buildScene(const Topl_SceneGraph* sceneGraph) override;
	void createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader) override;
private:
    void init(NATIVE_WINDOW hwnd) override;
    void createPipeline(void) override; // Marked for removal
	void render(void) override;

    Topl_Pipeline_GL4 m_pipeline;
	Topl_BufferAlloc_GL4 m_bufferAlloc;
	std::vector<Buffer_GL4> mBuffers;
	unsigned mMaxBuffID = 1;
};