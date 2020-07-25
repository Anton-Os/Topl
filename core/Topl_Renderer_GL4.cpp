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

GLuint Topl_TextureBindingAlloc_GL4::getAvailable(){
	if (!mIsInit) init();
	GLuint targetTextureB = 0;

	if (slotIndex > GL4_TEXTURE_BINDINGS_MAX)
		puts("Maximum Texture Bindings capacity exceeded!");
	else {
		targetTextureB = slots[slotIndex];
		slotIndex++;
	}

	return targetTextureB;
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
		return *(bufferPtrs + type); // We know the offset with the type argument
	}

	static void discoverBuffers(Buffer_GL4** dBuffers, std::vector<Buffer_GL4> * bufferVector, unsigned id) {
		//TODO No error checks for duplicate buffers are provided, bufferVector needs to be vetted first
		for (std::vector<Buffer_GL4>::iterator currentBuff = bufferVector->begin(); currentBuff < bufferVector->end(); currentBuff++)
			if (currentBuff->targetID == id)
				*(dBuffers + currentBuff->type) = &(*currentBuff); // Type indicates 
	}

	static void setTextureProperties(GLenum type, TEX_Mode m) {
		switch (m) {
		case TEX_Wrap:
			glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		default:
			break;
		}
		return;
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
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.4f, 0.4f, 0.9f, 1.0f);

}

void Topl_Renderer_GL4::buildScene(const Topl_SceneManager* sMan){

	glGenVertexArrays(GL4_VERTEX_ARRAY_MAX, &m_pipeline.vertexDataLayouts[0]);

	for (unsigned g = 0; g < sMan->getGeoCount(); g++) { // Slot index will signify how many buffers exist
		tpl_gEntity_cptr geoTarget_ptr = sMan->getGeoNode(g + 1); // ID values begin at 1
		Geo_RenderObj* geoTarget_renderObj = (Geo_RenderObj*)geoTarget_ptr->mRenderObj;
		
		perVertex_cptr geoTarget_perVertexData = geoTarget_renderObj->getPerVertexData();
		ui_cptr geoTarget_iData = geoTarget_renderObj->getIData();
		vec3f_cptr geoTarget_position = geoTarget_ptr->getPos();

		_GL4::UniformBlock block = _GL4::UniformBlock(geoTarget_position);
		mBuffers.push_back(Buffer_GL4(g + 1, BUFF_Const_vec3f, m_bufferAlloc.getAvailable()));
		glBindBuffer(GL_UNIFORM_BUFFER, mBuffers[mBuffers.size() - 1].buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(_GL4::UniformBlock), &block, GL_STATIC_DRAW);

		mBuffers.push_back(Buffer_GL4(g + 1, BUFF_Index_UI, m_bufferAlloc.getAvailable(), geoTarget_renderObj->getICount()));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[mBuffers.size() - 1].buffer); // Gets the latest buffer for now
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, geoTarget_renderObj->getICount() * sizeof(unsigned), geoTarget_iData, GL_STATIC_DRAW);

		mBuffers.push_back(Buffer_GL4(g + 1, BUFF_Vertex_3F, m_bufferAlloc.getAvailable(), geoTarget_renderObj->getVCount()));
		glBindBuffer(GL_ARRAY_BUFFER, mBuffers[mBuffers.size() - 1].buffer); // Gets the latest buffer for now
		glBufferData(GL_ARRAY_BUFFER, geoTarget_renderObj->getVCount() * sizeof(Geo_PerVertexData), geoTarget_perVertexData, GL_STATIC_DRAW);

		mVAOs.push_back(VertexArray_GL4(g + 1, m_vertexArrayAlloc.getAvailable()));
		VertexArray_GL4* currentVAO_ptr = &mVAOs[mVAOs.size() - 1]; // Check to see if all parameters are valid
		glBindVertexArray(currentVAO_ptr->vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,
							   GL_FLOAT, GL_FALSE,
							   sizeof(Geo_PerVertexData), NULL
		);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2,
			GL_FLOAT, GL_FALSE,
			sizeof(Geo_PerVertexData), GL4_BUFFER_OFFSET(12)
			// &geoTarget_perVertexData->texCoord[0]
			// geoTarget_perVertexData + offsetof(Geo_PerVertexData, texCoord)
		);
		m_pipeline.layoutIndex++; // TODO: Figure this part out, mark for deletion

#ifdef RASTERON_H
	unsigned texCount = sMan->getTextures(g + 1, nullptr);
	if(texCount > 0){
		const Rasteron_Image* baseTex = sMan->getFirstTexture(g + 1);

		genTexture(baseTex, g + 1); // Add the method definition
	}
#endif
		mMaxGraphicsID = g + 1;
	}

	mSceneReady = true;
    return; // To be continued
}

#ifdef RASTERON_H

Rasteron_Image* Topl_Renderer_GL4::getFrame(){
	// Custom Image format creation
	Rasteron_Image* rstn_image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));

	rstn_image->width = WIN_WIDTH; // defined in native_os_def
	rstn_image->height = WIN_HEIGHT; // defined in native_os_def
	rstn_image->name = "framebuff"; // TODO: Make this incremental, i.e framebuff1 framebuff2

	rstn_image->data = (uint32_t*)malloc(rstn_image->width * rstn_image->height * sizeof(uint32_t));
	glReadPixels(0, 0, rstn_image->width, rstn_image->height, GL_RGBA, GL_UNSIGNED_BYTE, rstn_image->data);

	return rstn_image;
}

void Topl_Renderer_GL4::genTexture(const Rasteron_Image* image, unsigned id){
	// TODO: Check for format compatablitiy before this call, TEX_2D
	GLuint texture = m_textureBindingsAlloc.getAvailable();
	glBindTexture(GL_TEXTURE_2D, texture);

	_GL4::setTextureProperties(GL_TEXTURE_2D, TEX_Wrap); // Setting this here 

	// glTextureStorage2D(texture, 1, GL_RGBA32UI, image->width, image->height); // 1 mip level, second argument
	// glTextureSubImage2D(texture, 0, 0, 0, image->width, image->height, GL_RGBA, GL_UNSIGNED_INT, image->data);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	mTextures.push_back(Texture_GL4(id, TEX_2D, TEX_Wrap, texture));
}

#endif

void Topl_Renderer_GL4::update(const Topl_SceneManager* sMan){
	Buffer_GL4* targetBuff = nullptr;

	for (unsigned g = 0; g < sMan->getGeoCount(); g++) {
		tpl_gEntity_cptr geoTarget_ptr = sMan->getGeoNode(g + 1); // ids begin at 1 // Add safeguards!
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

	Buffer_GL4** bufferPtrs = (Buffer_GL4**)malloc(MAX_BUFFERS_PER_TARGET * sizeof(Buffer_GL4*));

	// for (unsigned id = 1; id <= mMaxGraphicsID; id++) {
	for (unsigned id = mMaxGraphicsID; id >= 1; id--) {
		for (std::vector<VertexArray_GL4>::iterator currentVAO = mVAOs.begin(); currentVAO < mVAOs.end(); currentVAO++)
			if (currentVAO->targetID == id)
				glBindVertexArray(currentVAO->vao);
			else
				continue; // If it continues all the way through error has occured

		// Buffer binding step
		_GL4::discoverBuffers(bufferPtrs, &mBuffers, id); // Untested! Make sure this works!

		Buffer_GL4* indexBuff = _GL4::findBuffer(BUFF_Index_UI, bufferPtrs, MAX_BUFFERS_PER_TARGET);
		Buffer_GL4* vertexBuff = _GL4::findBuffer(BUFF_Vertex_3F, bufferPtrs, MAX_BUFFERS_PER_TARGET);
		Buffer_GL4* blockBuff = _GL4::findBuffer(BUFF_Const_vec3f, bufferPtrs, MAX_BUFFERS_PER_TARGET);

		if (GLuint blockIndex = glGetUniformBlockIndex(m_pipeline.shaderProg, "Block") != GL_INVALID_INDEX) {
			glUniformBlockBinding(m_pipeline.shaderProg, blockIndex, DEFAULT_BLOCK_BINDING);
			glBindBufferBase(GL_UNIFORM_BUFFER, DEFAULT_BLOCK_BINDING, blockBuff->buffer);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuff->buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff->buffer);

		for (unsigned t = 0; t < mTextures.size(); t++) {
			if (mTextures.at(t).targetID > id) break; // This means we have passed it in sequence
			else if (mTextures.at(t).targetID == id) {
				glBindTexture(GL_TEXTURE_2D, mTextures.at(t).texture);
				break;
			}
		}

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