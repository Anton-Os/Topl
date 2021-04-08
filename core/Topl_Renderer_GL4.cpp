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

namespace _GL4 {
	static GLenum getFormatFromShaderVal(enum SHDR_ValueType type){
		GLenum format;

		switch(type) {
		case SHDR_float_vec4:
		case SHDR_float_vec3:
		case SHDR_float_vec2:
		case SHDR_float:
			format = GL_FLOAT;
			break;
		case SHDR_double_vec4:
		case SHDR_double_vec3:
		case SHDR_double_vec2:
		case SHDR_double:
			format = GL_DOUBLE;
			break;
		case SHDR_uint_vec4:
		case SHDR_uint_vec3:
		case SHDR_uint_vec2:
		case SHDR_uint:
			format = GL_UNSIGNED_INT;
			break;
		case SHDR_int_vec4:
		case SHDR_int_vec3:
		case SHDR_int_vec2:
		case SHDR_int:
			format = GL_INT;
			break;
		default:
			puts("GL4 Shader Input Type Not Supported!");
			break;
		}

		return format;
	}

	static GLint getSizeFromShaderVal(enum SHDR_ValueType type){
		GLint size = 0;

		switch(type){
		case SHDR_float_vec4:
		case SHDR_double_vec4:
		case SHDR_uint_vec4:
		case SHDR_int_vec4:
			size = 4;
			break;
		case SHDR_float_vec3:
		case SHDR_double_vec3:
		case SHDR_uint_vec3:
		case SHDR_int_vec3:
			size = 3;
			break;
		case SHDR_float_vec2:
		case SHDR_double_vec2:
		case SHDR_uint_vec2:
		case SHDR_int_vec2:
			size = 2;
			break;
		case SHDR_float:
		case SHDR_double:
		case SHDR_uint:
		case SHDR_int:
			size = 2;
			break;
		}

		return size;
	}

	// Shared Renderer Code!
	static unsigned getOffsetFromShaderVal(enum SHDR_ValueType type){
		unsigned offset = 0;

		switch(type) {
		case SHDR_float_vec4:
			offset = sizeof(float) * 4; break;
		case SHDR_float_vec3:
			offset = sizeof(float) * 3; break;
		case SHDR_float_vec2:
			offset = sizeof(float) * 2; break;
		case SHDR_float:
			offset = sizeof(float); break;
		case SHDR_double_vec4:
			offset = sizeof(double) * 4; break;
		case SHDR_double_vec3:
			offset = sizeof(double) * 3; break;
		case SHDR_double_vec2:
			offset = sizeof(double) * 2; break;
		case SHDR_double:
			offset = sizeof(double); break;
		case SHDR_uint_vec4:
			offset = sizeof(unsigned) * 4; break;
		case SHDR_uint_vec3:
			offset = sizeof(unsigned) * 3;  break;
		case SHDR_uint_vec2:
			offset = sizeof(unsigned) * 2; break;
		case SHDR_uint:
			offset = sizeof(unsigned); break;
		case SHDR_int_vec4:
			offset = sizeof(int) * 4; break;
		case SHDR_int_vec3:
			offset = sizeof(int) * 3; break;
		case SHDR_int_vec2:
			offset = sizeof(int) * 2; break;
		case SHDR_int:
			offset = sizeof(int); break;
		default:
			puts("GL4 Type Shader Input Type Not Supported!");
			break;
		}

		return offset;
	}

	static Buffer_GL4* findBuffer(enum BUFF_Type type, Buffer_GL4** buffer_ptrs, unsigned short count) {
		return *(buffer_ptrs + type); // We know the offset with the type argument
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

static inline void swapBuffers_win(HDC* windowDC) { SwapBuffers(*(windowDC)); }

static void cleanup_win(HWND* hwnd, HDC* windowDC, HGLRC* hglrc){
  wglMakeCurrent(NULL, NULL);
	wglDeleteContext(*(hglrc));

	ReleaseDC(*(hwnd), *(windowDC));
}
#elif defined(__linux__)
static void init_linux(GLXContext graphicsContext, Display* display, Window* window){
	GLint visualInfoAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None }; // Copy from Platform.cpp createWindow()
	XVisualInfo* visualInfo = glXChooseVisual(display, 0, visualInfoAttribs); // Copy from Platform.cpp createWindow()

	graphicsContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE); // Object might be moved inside _Native_Platform_Context
	glXMakeCurrent(display, *window, graphicsContext);
}

static void swapBuffers_linux(Display* display, Window* window){ glXSwapBuffers(display, *window); }

static void cleanup_linux(Display* display, GLXContext graphicsContext){ glXDestroyContext(display, graphicsContext);}
#endif


Topl_Renderer_GL4::~Topl_Renderer_GL4() {
#ifdef _WIN32
	cleanup_win(&mNativeContext.window, &mNativeContext.windowDevice_Ctx, &mNativeContext.GL_ctx);
#elif defined(__linux__)
	cleanup_linux(mNativeContext.display, mNativeContext.GL_ctx);
#endif
}


void Topl_Renderer_GL4::init(NATIVE_WINDOW hwnd){
	mNativeContext.window = hwnd;
#ifdef _WIN32
    init_win(&mNativeContext.window, &mNativeContext.windowDevice_Ctx, &mNativeContext.GL_ctx);
#elif defined(__linux__)
	init_linux(mNativeContext.GL_ctx, mNativeContext.display, mNativeContext.window_ptr);
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

	const Topl_Shader* primaryShader = findShader(mPrimaryShaderType);
	std::vector<uint8_t> blockBytes; // For constant and uniform buffer updates

	glGenVertexArrays(GL4_VERTEX_ARRAY_MAX, &m_pipeline.vertexDataLayouts[0]);

	// Generates object for single scene block buffer
	if (primaryShader->genPerSceneDataBlock(sMan, &blockBytes)) {
		mBuffers.push_back(Buffer_GL4(m_bufferAlloc.getAvailable()));
		glBindBuffer(GL_UNIFORM_BUFFER, mBuffers.back().buffer);
		unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	for (unsigned g = 0; g < sMan->getGeoCount(); g++) { // Slot index will signify how many buffers exist
		unsigned currentRenderID = g + 1;
		topl_geoComponent_cptr geoTarget_ptr = sMan->getGeoComponent(currentRenderID - 1); // ids begin at 1, conversion is required
		Geo_RenderObj* geoTarget_renderObj = (Geo_RenderObj*)geoTarget_ptr->getRenderObj();

		perVertex_cptr geoTarget_perVertexData = geoTarget_renderObj->getPerVertexData();
		ui_cptr geoTarget_iData = geoTarget_renderObj->getIndexData();

		// Geo component block implementation
		if (primaryShader->genPerGeoDataBlock(geoTarget_ptr, &blockBytes)) {
			mBuffers.push_back(Buffer_GL4(currentRenderID, BUFF_Renderable_Block, m_bufferAlloc.getAvailable()));
			glBindBuffer(GL_UNIFORM_BUFFER, mBuffers.back().buffer);
			unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
			glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		// Index creation procedures
		if (geoTarget_iData != nullptr) {
			mBuffers.push_back(Buffer_GL4(currentRenderID, BUFF_Index_UI, m_bufferAlloc.getAvailable(), geoTarget_renderObj->getIndexCount()));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers.back().buffer); // Gets the latest buffer for now
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, geoTarget_renderObj->getIndexCount() * sizeof(unsigned), geoTarget_iData, GL_STATIC_DRAW);
		} else mBuffers.push_back(Buffer_GL4(currentRenderID, BUFF_Index_UI, m_bufferAlloc.getAvailable(), 0)); // 0 indicates empty buffer

		mBuffers.push_back(Buffer_GL4(currentRenderID, BUFF_Vertex_Type, m_bufferAlloc.getAvailable(), geoTarget_renderObj->getVertexCount()));
		glBindBuffer(GL_ARRAY_BUFFER, mBuffers.back().buffer); // Gets the latest buffer for now
		glBufferData(GL_ARRAY_BUFFER, geoTarget_renderObj->getVertexCount() * sizeof(Geo_PerVertexData), geoTarget_perVertexData, GL_STATIC_DRAW);

		mVAOs.push_back(VertexArray_GL4(currentRenderID, m_vertexArrayAlloc.getAvailable()));
		VertexArray_GL4* currentVAO_ptr = &mVAOs.back(); // Check to see if all parameters are valid
		glBindVertexArray(currentVAO_ptr->vao);

		GLsizei inputElementOffset = 0;
		for(unsigned i = 0; i < primaryShader->getInputCount(); i++){
			const Shader_Type* shaderType = primaryShader->getInputAtIndex(i);

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,
				_GL4::getSizeFromShaderVal(shaderType->type),
				_GL4::getFormatFromShaderVal(shaderType->type),
				GL_FALSE,
				sizeof(Geo_PerVertexData),
				(inputElementOffset != 0) ? GL4_BUFFER_OFFSET(inputElementOffset) : NULL
			);

			inputElementOffset += _GL4::getOffsetFromShaderVal(shaderType->type);
		}

#ifdef RASTERON_H
		// TODO: Add support for multiple textures
		const Rasteron_Image* baseTex = sMan->getFirstTexture(geoTarget_ptr->getName());
		if(baseTex != nullptr) genTexture(baseTex, currentRenderID); // Add the method definition
#endif
		mMainRenderIDs = currentRenderID; // Sets main graphics ID's to max value of currentRenderID
	}

	GLint blockCount; glGetProgramiv(m_pipeline.shaderProg, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	if (blockCount == TOPL_SINGLE_BLOCK_COUNT) // Render uniforms supported
		glUniformBlockBinding(m_pipeline.shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
	else if (blockCount == TOPL_FULL_BLOCK_COUNT) { // Render and Scene uniforms supported
		glUniformBlockBinding(m_pipeline.shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
		glUniformBlockBinding(m_pipeline.shaderProg, SCENE_BLOCK_INDEX, SCENE_BLOCK_BINDING);
	}
	else {
		mSceneReady = false; // Error
		return;
	}

	mSceneReady = true;
    return; // To be continued
}

#ifdef RASTERON_H

// EXPERIMENTAL SCREEN CAPTURE CODE!
Rasteron_Image* Topl_Renderer_GL4::getFrame(){
	// Custom Image format creation
	Rasteron_Image* rstn_image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));

	rstn_image->width = TOPL_WIN_WIDTH; // defined in native_os_def
	rstn_image->height = TOPL_WIN_HEIGHT; // defined in native_os_def
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	mTextures.push_back(Texture_GL4(id, TEX_2D, TEX_Wrap, texture));
}

#endif

void Topl_Renderer_GL4::update(const Topl_SceneManager* sMan){
	const Topl_Shader* primaryShader = findShader(mPrimaryShaderType);
	std::vector<uint8_t> blockBytes;
	Buffer_GL4* targetBuff = nullptr;

	if (primaryShader->genPerSceneDataBlock(sMan, &blockBytes) && mBuffers.front().targetID == SPECIAL_SCENE_RENDER_ID) {
		glBindBuffer(GL_UNIFORM_BUFFER, mBuffers.front().buffer);
		unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
	}

	for (unsigned g = 0; g < sMan->getGeoCount(); g++) {
		unsigned currentRenderID = g + 1;
		topl_geoComponent_cptr geoTarget_ptr = sMan->getGeoComponent(currentRenderID - 1); // ids begin at 1, conversion is required
		if (primaryShader->genPerGeoDataBlock(geoTarget_ptr, &blockBytes)) {
			for (std::vector<Buffer_GL4>::iterator currentBuff = mBuffers.begin(); currentBuff < mBuffers.end(); currentBuff++)
				if (currentBuff->targetID == currentRenderID && currentBuff->type == BUFF_Renderable_Block) targetBuff = &(*currentBuff);

			if (targetBuff == nullptr)
				puts("Block buffer could not be located! ");
			else {
				glBindBuffer(GL_UNIFORM_BUFFER, targetBuff->buffer);
				unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
				glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
			}
		}
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	mSceneReady = true;
	return;
}

void Topl_Renderer_GL4::pipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader){
	GLint result;
	const char* sourceStr_ptr;

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

void Topl_Renderer_GL4::pipeline(topl_shader_cptr vertexShader, topl_shader_cptr fragShader, topl_shader_cptr tessCtrlShader, topl_shader_cptr tessEvalShader, topl_shader_cptr geomShader, topl_shader_cptr compShader){
	return; // TODO: Implement this code block
}

void Topl_Renderer_GL4::render(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLenum drawType;
	switch(mDrawType){
	case DRAW_Triangles:
		drawType = GL_TRIANGLES;
		break;
	case DRAW_Points:
		drawType = GL_POINTS;
		break;
	case DRAW_Lines:
		drawType = GL_LINES;
		break;
	default:
		puts("Draw type not supported yet!");
		return;
	}

	if (mBuffers.front().targetID == SPECIAL_SCENE_RENDER_ID)
		glBindBufferBase(GL_UNIFORM_BUFFER, SCENE_BLOCK_BINDING, mBuffers.front().buffer);

	Buffer_GL4** buffer_ptrs = (Buffer_GL4**)malloc(MAX_BUFFERS_PER_TARGET * sizeof(Buffer_GL4*));

	// Rendering Loop!
	for (unsigned id = 1; id <= mMainRenderIDs; id++) {
		for (std::vector<VertexArray_GL4>::iterator currentVAO = mVAOs.begin(); currentVAO < mVAOs.end(); currentVAO++)
			if (currentVAO->targetID == id)
				glBindVertexArray(currentVAO->vao);
			else
				continue; // If it continues all the way through error has occured

		// Buffer discovery and binding step
		_GL4::discoverBuffers(buffer_ptrs, &mBuffers, id);

		Buffer_GL4* renderBlockBuff = _GL4::findBuffer(BUFF_Renderable_Block, buffer_ptrs, MAX_BUFFERS_PER_TARGET);
		if (renderBlockBuff != nullptr)
			glBindBufferBase(GL_UNIFORM_BUFFER, RENDER_BLOCK_BINDING, renderBlockBuff->buffer);

		Buffer_GL4* vertexBuff = _GL4::findBuffer(BUFF_Vertex_Type, buffer_ptrs, MAX_BUFFERS_PER_TARGET);
		Buffer_GL4* indexBuff = _GL4::findBuffer(BUFF_Index_UI, buffer_ptrs, MAX_BUFFERS_PER_TARGET);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuff->buffer);
		if(indexBuff != nullptr) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff->buffer);

		for (unsigned t = 0; t < mTextures.size(); t++) {
			if (mTextures.at(t).targetID > id) break; // This means we have passed it in sequence
			else if (mTextures.at(t).targetID == id) {
				glBindTexture(GL_TEXTURE_2D, mTextures.at(t).texture);
				break;
			}
		}

		if (indexBuff != nullptr && indexBuff->count != 0) glDrawElements(drawType, indexBuff->count, GL_UNSIGNED_INT, (void*)0);
		else glDrawArrays(drawType, 0, vertexBuff->count); // When no indices are present

		// Unbinding
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	free(buffer_ptrs);
#ifdef _WIN32 // Swap buffers in windows
	swapBuffers_win(&mNativeContext.windowDevice_Ctx);
#elif defined(__linux__)
	swapBuffers_linux(mNativeContext.display, mNativeContext.window_ptr);
#endif
}
