#include "Topl_Renderer.hpp"

class Topl_DataAlloc_GL4 {
public:
	Topl_DataAlloc_GL4() {}

	virtual GLuint getAvailable() = 0;
protected:
	bool mIsInit = false;
	virtual void init() = 0;
	// slots and slotIndex have to probably remain separate
};

// Buffer Object Allocation Helpers

struct Buffer_GL4 : public Buffer {
	Buffer_GL4() : Buffer() {}
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b) : Buffer(id, t){
		buffer = b;
	}
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b, unsigned c) : Buffer(id, t, c) {
		buffer = b;
	}

	GLuint buffer; // OpenGL specific
};

#define GL4_BUFFER_MAX 1024

class Topl_BufferAlloc_GL4 : public Topl_DataAlloc_GL4 { // derived class
public:
	Topl_BufferAlloc_GL4(){}

	GLuint getAvailable() override;
private:
	void init() override { glGenBuffers(GL4_BUFFER_MAX, &slots[0]); mIsInit = true; }

	GLuint slots[GL4_BUFFER_MAX];
	unsigned slotIndex = 0; // May need to be zero
};

// Vertex Array Object Allocation Helpers

struct VertexArray_GL4 : public RenderTargetObject {
	VertexArray_GL4() : RenderTargetObject() {}
	VertexArray_GL4(unsigned id, GLuint v, GLint sz, GLenum t) : RenderTargetObject(id){
		vao = v; size = sz; type = t;
	}
	VertexArray_GL4(unsigned id, GLuint v, GLint sz, GLenum t, GLuint i, GLboolean n, GLsizei st) : RenderTargetObject(id){
		vao = v; size = sz; type = t; index = i; normalized = n; stride = st;
	}

	GLuint vao;
	GLuint index = 0;
  	GLint size;
  	GLenum type;
  	GLboolean normalized = GL_FALSE;
  	GLsizei stride = 0;
};

#define GL4_VERTEX_ARRAY_MAX 1024

class Topl_VertexArrayAlloc_GL4 : public Topl_DataAlloc_GL4 { // derived class
public:
	Topl_VertexArrayAlloc_GL4(){}

	GLuint getAvailable() override;
private:
	void init() override { glGenVertexArrays(GL4_VERTEX_ARRAY_MAX, &slots[0]); mIsInit = true; }

	GLuint slots[GL4_VERTEX_ARRAY_MAX];
	unsigned slotIndex = 0; // May need to be zero
};

struct Topl_Pipeline_GL4 {
	GLuint vertexDataLayouts[GL4_VERTEX_ARRAY_MAX];
	unsigned layoutIndex = 0;

	GLuint vShader; // Vertex Shader // Make to vector
	GLuint fShader; // Fragment Shader // Make to vector
	GLuint shaderProg;
};

class Topl_Renderer_GL4 : public Topl_Renderer {
public:
    Topl_Renderer_GL4(HWND hwnd){ init(hwnd); }
	~Topl_Renderer_GL4();

    void buildScene(const Topl_SceneManager* sceneGraph) override;
	void createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader) override;
private:
    void init(NATIVE_WINDOW hwnd) override;
	void update(const Topl_SceneManager* sceneGraph) override;
	void render(void) override;

    Topl_Pipeline_GL4 m_pipeline;
	Topl_BufferAlloc_GL4 m_bufferAlloc;
	std::vector<Buffer_GL4> mBuffers;
	Topl_VertexArrayAlloc_GL4 m_vertexArrayAlloc;
	std::vector<VertexArray_GL4> mVAOs;
	unsigned mMaxBuffID = 1;
};