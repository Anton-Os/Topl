#include "FileIO.hpp"

#include "Topl_Renderer_GL4.hpp"

// This code is platform independent

namespace _GL4 {

}

#ifdef _WIN32
static void init_win(const HWND* hwnd, HDC* windowDC, HGLRC* hglrc){
    // Creates an HDC based on the window
    *(windowDC) = GetDC(*(hwnd));

    // Pixel format descriptor stuff
    PIXELFORMATDESCRIPTOR pixDescript, *pixDescript_ptr;
	pixDescript_ptr = &pixDescript;
	int pixFrmt;

	pixDescript.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixDescript.nVersion = 1;
	pixDescript.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixDescript.dwLayerMask = PFD_MAIN_PLANE;
	pixDescript.iPixelType = PFD_TYPE_RGBA;
	pixDescript.cColorBits = 24;

	pixFrmt = ChoosePixelFormat(*(windowDC), pixDescript_ptr);
	BOOL pixFrmtChk = SetPixelFormat(*(windowDC), pixFrmt, pixDescript_ptr);

    // wgl initialization functions
    *(hglrc) = wglCreateContext(*(windowDC));
    wglMakeCurrent(*(windowDC), *(hglrc));
}

static void render_win(HDC* windowDC) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	SwapBuffers(*(windowDC));
}

static void cleanup_win(HWND* hwnd, HDC* windowDC, HGLRC* hglrc){
    wglMakeCurrent(NULL, NULL);
	wglDeleteContext(*(hglrc));

	ReleaseDC(*(hwnd), *(windowDC));
}

#endif


Topl_Renderer_GL4::~Topl_Renderer_GL4() {
#ifdef _WIN32
	cleanup_win(m_native.window, &m_native.windowDevice_Ctx, &m_native.GL_Ctx);
#endif
}


void Topl_Renderer_GL4::init(NATIVE_WINDOW hwnd){
	m_native.window = &hwnd;
#ifdef _WIN32
    init_win(m_native.window, &m_native.windowDevice_Ctx, &m_native.GL_Ctx);
#endif
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST); // Make these customizable
	glDepthFunc(GL_LESS); // Make these customizable
	glClearColor(0.4f, 0.4f, 0.9f, 1.0f);

	// Topl_Renderer_GL4::createPipeline(); // Remove from here
}

void Topl_Renderer_GL4::buildScene(const Topl_SceneGraph* sceneGraph){

	glGenBuffers(GL4_BUFFER_CAPACITY, &m_bufferData.slots[0]);
	// Move this data to the buffer vector

	for(unsigned g = 0; g < sceneGraph->getGeoCount(); g++) { // Scalable version soon
		tpl_gEntity_cptr gRect1_ptr = sceneGraph->getGeoNode(g + 1);
		vec3f_cptr gRect1_vData = gRect1_ptr->mRenderObj->getVData();

		glBindBuffer(GL_ARRAY_BUFFER, m_bufferData.slots[0]); // Make this scalable
		glBufferData(GL_ARRAY_BUFFER, gRect1_ptr->mRenderObj->getVCount() * sizeof(Eigen::Vector3f), gRect1_vData, GL_STATIC_DRAW);
	}

	// This is part of pipeline generation, relocate later
	glGenVertexArrays(GL4_VERTEX_ARRAY_CAPACITY, &m_pipeline.vertexDataLayouts[0]);
	glBindVertexArray(m_pipeline.vertexDataLayouts[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	mSceneReady = true;

    return; // To be continued
}

void Topl_Renderer_GL4::createPipeline(void){
	GLint result;
	const char* sourceStr_ptr;

	m_pipeline.vShader = glCreateShader(GL_VERTEX_SHADER);

	std::string vertexShaderSrc = readFile("C:\\AntonDocs\\Codex\\Ao-Project\\Topl-Skeleton\\MSVC_BUILD_2\\Debug\\VertexShader.glsl");
	sourceStr_ptr = vertexShaderSrc.c_str();
	if (!vertexShaderSrc.empty()) glShaderSource(m_pipeline.vShader, 1, &sourceStr_ptr, NULL);
	else {
		mPipelineReady = false;
		puts("Vertex shader file not found");
		return;
	}

	glCompileShader(m_pipeline.vShader);

	glGetShaderiv(m_pipeline.vShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		mPipelineReady = false;
		puts("Vertex shader compilation failed\n"); // Add more robust error checking
		
		GLint maxLen;
		glGetShaderiv(m_pipeline.vShader, GL_INFO_LOG_LENGTH, &maxLen);
		
		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetShaderInfoLog(m_pipeline.vShader, maxLen, &maxLen, errorLog);
		puts(errorLog);
		free(errorLog);

		return;
	}

	m_pipeline.fShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string fragShaderSrc = readFile("C:\\AntonDocs\\Codex\\Ao-Project\\Topl-Skeleton\\MSVC_BUILD_2\\Debug\\FragShader.glsl");
	sourceStr_ptr = fragShaderSrc.c_str();
	if (!fragShaderSrc.empty()) glShaderSource(m_pipeline.fShader, 1, &sourceStr_ptr, NULL);
	else {
		mPipelineReady = false;
		puts("Fragment shader file not found");
		return;
	}

	glCompileShader(m_pipeline.fShader);

	glGetShaderiv(m_pipeline.fShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		mPipelineReady = false;
		puts("Fragment shader compilation failed"); // Add more robust error checking

		GLint maxLen;
		glGetShaderiv(m_pipeline.fShader, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetShaderInfoLog(m_pipeline.fShader, maxLen, &maxLen, errorLog);
		puts(errorLog);
		free(errorLog);

		return;
	}

	// Program creation code

	m_pipeline.shaderProg = glCreateProgram();
	glAttachShader(m_pipeline.shaderProg, m_pipeline.vShader);
	glAttachShader(m_pipeline.shaderProg, m_pipeline.fShader);
	glLinkProgram(m_pipeline.shaderProg);

	glGetProgramiv(m_pipeline.shaderProg, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		mPipelineReady = false;
		puts("Shader program failed to link"); // Add more robust error checking

		GLint maxLen;
		glGetProgramiv(m_pipeline.shaderProg, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetProgramInfoLog(m_pipeline.shaderProg, maxLen, &maxLen, errorLog);
		puts(errorLog);
		free(errorLog);

		return;
	} else {
		// Always detach after successful link
		glDetachShader(m_pipeline.shaderProg, m_pipeline.vShader);
		glDetachShader(m_pipeline.shaderProg, m_pipeline.fShader);
	}

	glUseProgram(m_pipeline.shaderProg); // Move this later
	mPipelineReady = true;
}

void Topl_Renderer_GL4::createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader){
	GLint result;
	const char* sourceStr_ptr;

	// Check for correct type
	if (vertexShader->getType() != SHDR_Vertex) {
		mPipelineReady = false;
		puts("Incorrect shader type provided for vertex shader");
		return;
	}

	// Vertex shader creation and valid file checking
	m_pipeline.vShader = glCreateShader(GL_VERTEX_SHADER);

	std::string vertexShaderSrc = readFile(vertexShader->getFilePath());
	sourceStr_ptr = vertexShaderSrc.c_str();
	if (!vertexShaderSrc.empty()) glShaderSource(m_pipeline.vShader, 1, &sourceStr_ptr, NULL);
	else {
		mPipelineReady = false;
		puts("Vertex shader file not found");
		return;
	}

	// Vertex shader compilation and syntax checking
	glCompileShader(m_pipeline.vShader);

	glGetShaderiv(m_pipeline.vShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		mPipelineReady = false;
		puts("Vertex shader compilation failed\n"); // Add more robust error checking

		GLint maxLen;
		glGetShaderiv(m_pipeline.vShader, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetShaderInfoLog(m_pipeline.vShader, maxLen, &maxLen, errorLog);
		puts(errorLog);
		free(errorLog);

		return;
	}

	// Check for correct type
	if (fragShader->getType() != SHDR_Fragment) {
		mPipelineReady = false;
		puts("Incorrect shader type provided for fragment shader");
		return;
	}

	// Fragment shader creation and valid file checking
	m_pipeline.fShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string fragShaderSrc = readFile(fragShader->getFilePath());
	sourceStr_ptr = fragShaderSrc.c_str();
	if (!fragShaderSrc.empty()) glShaderSource(m_pipeline.fShader, 1, &sourceStr_ptr, NULL);
	else {
		mPipelineReady = false;
		puts("Fragment shader file not found");
		return;
	}

	// Fragment shader compilation and syntax checking
	glCompileShader(m_pipeline.fShader);

	glGetShaderiv(m_pipeline.fShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		mPipelineReady = false;
		puts("Fragment shader compilation failed"); // Add more robust error checking

		GLint maxLen;
		glGetShaderiv(m_pipeline.fShader, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetShaderInfoLog(m_pipeline.fShader, maxLen, &maxLen, errorLog);
		puts(errorLog);
		free(errorLog);

		return;
	}

	// Program creation code

	m_pipeline.shaderProg = glCreateProgram();
	glAttachShader(m_pipeline.shaderProg, m_pipeline.vShader);
	glAttachShader(m_pipeline.shaderProg, m_pipeline.fShader);
	glLinkProgram(m_pipeline.shaderProg);

	glGetProgramiv(m_pipeline.shaderProg, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		mPipelineReady = false;
		puts("Shader program failed to link"); // Add more robust error checking

		GLint maxLen;
		glGetProgramiv(m_pipeline.shaderProg, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetProgramInfoLog(m_pipeline.shaderProg, maxLen, &maxLen, errorLog);
		puts(errorLog);
		free(errorLog);

		return;
	}
	else {
		// Always detach after successful link
		glDetachShader(m_pipeline.shaderProg, m_pipeline.vShader);
		glDetachShader(m_pipeline.shaderProg, m_pipeline.fShader);
	}

	glUseProgram(m_pipeline.shaderProg); // Move this later
	mPipelineReady = true;
}

void Topl_Renderer_GL4::render(void){
#ifdef _WIN32
	render_win(&m_native.windowDevice_Ctx);
#endif  
}