#include "FileIO.hpp"

#include "Topl_Renderer_GL4.hpp"

// This code is platform independent

GLuint Topl_BufferAlloc_GL4::getAvailable() {
	if (!mIsInit) init();
	GLuint targetBuff = 0;

	if (slotIndex > GL4_BUFFER_MAX)
		puts("Maximum buffer capacity exceeded!");
	else {
		targetBuff = slots[slotIndex];
		slotIndex++;
	}

	return targetBuff;
}

GLuint Topl_VertexArrayAlloc_GL4::getAvailable(){
	if (!mIsInit) init();
	GLuint targetVAO = 0;

	if (slotIndex > GL4_VERTEX_ARRAY_MAX)
		puts("Maximum VAO capacity exceeded!");
	else {
		targetVAO = slots[slotIndex];
		slotIndex++;
	}

	return targetVAO;
}

#define DEFAULT_BLOCK_BINDING 0

namespace _GL4 {
	struct UniformBlock {
		UniformBlock(vec3f_cptr v) {
			offset = *(v);
		}
		Eigen::Vector3f offset;
	};

	static Buffer_GL4* findBuffer(enum BUFF_Type type, Buffer_GL4** bufferPtrs, unsigned short count) {
		Buffer_GL4* currentBuff = nullptr;

		for (unsigned short b = 0; b < count; b++) {
			currentBuff = *(bufferPtrs + b);
			if (currentBuff->type == type) break; // Correct buffer was found
			else currentBuff = nullptr;
		}

		return currentBuff; // Try to optimize the code above
	}
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

	// Need to loop through all created objects
	// for(unsigned b = 0; b )
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	SwapBuffers(*(windowDC));
}

static inline void swapBuffers_win(HDC* windowDC) { SwapBuffers(*(windowDC)); }

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

void Topl_Renderer_GL4::buildScene(const Topl_SceneManager* sceneGraph){

	// glGenBuffers(GL4_BUFFER_CAPACITY, &m_bufferAlloc.slots[0]);
	glGenVertexArrays(GL4_VERTEX_ARRAY_MAX, &m_pipeline.vertexDataLayouts[0]);

	for (unsigned g = 0; g < sceneGraph->getGeoCount(); g++) { // Slot index will signify how many buffers exist
		tpl_gEntity_cptr geoTarget_ptr = sceneGraph->getGeoNode(g + 1); // ID values begin at 1
		vec3f_cptr geoTarget_vData = geoTarget_ptr->mRenderObj->getVData();
		ui_cptr geoTarget_iData = geoTarget_ptr->mRenderObj->getIData();
		vec3f_cptr geoTarget_position = geoTarget_ptr->getPos();

		// mBuffers.push_back(Buffer_GL4(g + 1, BUFF_Const_vec3f, m_bufferAlloc.getAvailable(), 1));
		_GL4::UniformBlock block = _GL4::UniformBlock(geoTarget_position);
		mBuffers.push_back(Buffer_GL4(g + 1, BUFF_Const_vec3f, m_bufferAlloc.getAvailable()));
		glBindBuffer(GL_UNIFORM_BUFFER, mBuffers[mBuffers.size() - 1].buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(_GL4::UniformBlock), &block, GL_STATIC_DRAW);
		// GLuint blockIndex = glGetUniformBlockIndex(m_pipeline.shaderProg, "ShaderBlock");
		// glUniformBlockBinding(m_pipeline.shaderProg, blockIndex, DEFAULT_BLOCK_BINDING);
		// glBindBufferBase(GL_UNIFORM_BUFFER, DEFAULT_BLOCK_BINDING, mBuffers[mBuffers.size() - 1].buffer);

		mBuffers.push_back(Buffer_GL4(g + 1, BUFF_Index_UI, m_bufferAlloc.getAvailable(), geoTarget_ptr->mRenderObj->getICount()));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[mBuffers.size() - 1].buffer); // Gets the latest buffer for now
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, geoTarget_ptr->mRenderObj->getICount() * sizeof(unsigned), geoTarget_iData, GL_STATIC_DRAW);

		mBuffers.push_back(Buffer_GL4(g + 1, BUFF_Vertex_3F, m_bufferAlloc.getAvailable(), geoTarget_ptr->mRenderObj->getVCount()));
		glBindBuffer(GL_ARRAY_BUFFER, mBuffers[mBuffers.size() - 1].buffer); // Gets the latest buffer for now
		glBufferData(GL_ARRAY_BUFFER, geoTarget_ptr->mRenderObj->getVCount() * sizeof(Eigen::Vector3f), geoTarget_vData, GL_STATIC_DRAW);

		mVAOs.push_back(VertexArray_GL4(g + 1, m_vertexArrayAlloc.getAvailable(), 3, GL_FLOAT));
		VertexArray_GL4* currentVAO_ptr = &mVAOs[mVAOs.size() - 1]; // Check to see if all parameters are valid
		glBindVertexArray(currentVAO_ptr->vao);
		glEnableVertexAttribArray(currentVAO_ptr->index);
		glVertexAttribPointer( currentVAO_ptr->index, 
							   currentVAO_ptr->size, 
							   currentVAO_ptr->type, 
							   currentVAO_ptr->normalized, 
							   currentVAO_ptr->stride,
							   NULL
		);

		m_pipeline.layoutIndex++;

		mMaxBuffID = g + 1;
	}

	mSceneReady = true;

    return; // To be continued
}

void Topl_Renderer_GL4::update(const Topl_SceneManager* sceneGraph){
	Buffer_GL4* targetBuff = nullptr;

	for (unsigned g = 0; g < sceneGraph->getGeoCount(); g++) {
		tpl_gEntity_cptr geoTarget_ptr = sceneGraph->getGeoNode(g + 1); // ids begin at 1 // Add safeguards!
		vec3f_cptr geoTarget_position = geoTarget_ptr->getPos();

		_GL4::UniformBlock block = _GL4::UniformBlock(geoTarget_position);

		for (std::vector<Buffer_GL4>::iterator currentBuff = mBuffers.begin(); currentBuff < mBuffers.end(); currentBuff++)
			if (currentBuff->targetID == g + 1 && currentBuff->type == BUFF_Const_vec3f) {
				targetBuff = &(*currentBuff);
				break;
			}

		if (targetBuff == nullptr) {
			puts("Position const buffer could not be located! ");
			return;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, targetBuff->buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(_GL4::UniformBlock), &block, GL_STATIC_DRAW);
	}

	mSceneReady = true;
	return;
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Buffer_GL4** bufferPtrs = (Buffer_GL4**)malloc(MAX_BUFFER_TYPES * sizeof(Buffer_GL4*));

	for (unsigned id = 1; id <= mMaxBuffID; id++) {
		// Vertex array must be bound first! Ha!
		for (std::vector<VertexArray_GL4>::iterator currentVAO = mVAOs.begin(); currentVAO < mVAOs.end(); currentVAO++)
			if (currentVAO->targetID == id)
				glBindVertexArray(currentVAO->vao);
			else
				continue; // If it continues all the way through error has occured

		// Buffer binding step
		unsigned bOffset = 0; // Populates the bufferPtrs structure
		for (std::vector<Buffer_GL4>::iterator currentBuff = mBuffers.begin(); currentBuff < mBuffers.end(); currentBuff++)
			if (currentBuff->targetID == id)
				if (bOffset >= MAX_BUFFER_TYPES) break;
				else {
					*(bufferPtrs + bOffset) = &(*currentBuff);
					bOffset++;
				} // bOffset will finally indicate how many buffers were located

		Buffer_GL4* indexBuff = _GL4::findBuffer(BUFF_Index_UI, bufferPtrs, bOffset);
		Buffer_GL4* vertexBuff = _GL4::findBuffer(BUFF_Vertex_3F, bufferPtrs, bOffset);
		Buffer_GL4* blockBuff = _GL4::findBuffer(BUFF_Const_vec3f, bufferPtrs, bOffset);

		if (GLuint blockIndex = glGetUniformBlockIndex(m_pipeline.shaderProg, "Block") != GL_INVALID_INDEX) {
			glUniformBlockBinding(m_pipeline.shaderProg, blockIndex, DEFAULT_BLOCK_BINDING);
			glBindBufferBase(GL_UNIFORM_BUFFER, DEFAULT_BLOCK_BINDING, blockBuff->buffer);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuff->buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff->buffer);

		glPointSize(10.0f); // For testing
		glLineWidth(3.0f); // For testing
		// glDrawArrays(GL_LINES, 0, vertexBuff->count);
		glDrawElements(GL_TRIANGLES, indexBuff->count, GL_UNSIGNED_INT, (void*)0);

		// Unbinding
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	free(bufferPtrs);
#ifdef _WIN32 // Swap buffers in windows
	swapBuffers_win(&m_native.windowDevice_Ctx);
#endif  
}