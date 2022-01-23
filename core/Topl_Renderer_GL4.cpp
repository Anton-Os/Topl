#include "Topl_Renderer_GL4.hpp"

namespace _GL4 {
	static GLenum getFormatFromShaderVal(enum SHDR_ValueType type){
		GLenum format;

		switch(type) {
		case SHDR_float_vec4: case SHDR_float_vec3: case SHDR_float_vec2: case SHDR_float:
			format = GL_FLOAT; break;
		case SHDR_double_vec4: case SHDR_double_vec3: case SHDR_double_vec2: case SHDR_double:
			format = GL_DOUBLE; break;
		case SHDR_uint_vec4: case SHDR_uint_vec3: case SHDR_uint_vec2: case SHDR_uint:
			format = GL_UNSIGNED_INT; break;
		case SHDR_int_vec4: case SHDR_int_vec3: case SHDR_int_vec2: case SHDR_int:
			format = GL_INT; break;
		default: puts("GL4 Shader Input Type Not Supported!"); break;
		}
		return format;
	}

	static GLint getSizeFromShaderVal(enum SHDR_ValueType type){
		GLint size = 0;

		switch(type){
		case SHDR_float_vec4: case SHDR_double_vec4: case SHDR_uint_vec4: case SHDR_int_vec4:
			size = 4; break;
		case SHDR_float_vec3: case SHDR_double_vec3: case SHDR_uint_vec3: case SHDR_int_vec3:
			size = 3; break;
		case SHDR_float_vec2: case SHDR_double_vec2: case SHDR_uint_vec2: case SHDR_int_vec2:
			size = 2; break;
		case SHDR_float: case SHDR_double: case SHDR_uint: case SHDR_int:
			size = 1; break;
		}
		return size;
	}

	// Shared Renderer Code!
	static unsigned getOffsetFromShaderVal(enum SHDR_ValueType type){
		unsigned offset = 0;

		switch(type) {
		case SHDR_float_vec4: offset = sizeof(float) * 4; break;
		case SHDR_float_vec3: offset = sizeof(float) * 3; break;
		case SHDR_float_vec2: offset = sizeof(float) * 2; break;
		case SHDR_float: offset = sizeof(float); break;
		case SHDR_double_vec4: offset = sizeof(double) * 4; break;
		case SHDR_double_vec3: offset = sizeof(double) * 3; break;
		case SHDR_double_vec2: offset = sizeof(double) * 2; break;
		case SHDR_double: offset = sizeof(double); break;
		case SHDR_uint_vec4: offset = sizeof(unsigned) * 4; break;
		case SHDR_uint_vec3: offset = sizeof(unsigned) * 3;  break;
		case SHDR_uint_vec2: offset = sizeof(unsigned) * 2; break;
		case SHDR_uint: offset = sizeof(unsigned); break;
		case SHDR_int_vec4: offset = sizeof(int) * 4; break;
		case SHDR_int_vec3: offset = sizeof(int) * 3; break;
		case SHDR_int_vec2: offset = sizeof(int) * 2; break;
		case SHDR_int: offset = sizeof(int); break;
		default:
			puts("GL4 Type Shader Input Type Not Supported!");
			break;
		}
		return offset;
	}

	static Buffer_GL4* findBuffer(enum BUFF_Type type, Buffer_GL4** buffer_ptrs, unsigned short count) {
		return *(buffer_ptrs + type); // offset is pre-determined by the type argument
	}

	static void discoverBuffers(Buffer_GL4** dBuffers, std::vector<Buffer_GL4> * bufferVector, unsigned id) {
		//TODO: No error checks for duplicate buffers are provided! bufferVector needs to be vetted first
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
		default: break;
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

	graphicsContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE); // Object might be moved inside _Native_Platfor_Context
	glXMakeCurrent(display, *window, graphicsContext);
}

static void swapBuffers_linux(Display* display, Window* window){ glXSwapBuffers(display, *window); }

static void cleanup_linux(Display* display, GLXContext graphicsContext){ glXDestroyContext(display, graphicsContext);}
#endif


Topl_Renderer_GL4::~Topl_Renderer_GL4() {
	glDeleteBuffers(GL4_BUFFER_MAX, &_bufferSlots[0]);
	glDeleteVertexArrays(GL4_VERTEX_ARRAY_MAX, &_vertexArraySlots[0]);
	glDeleteTextures(GL4_TEXTURE_BINDINGS_MAX, &_textureSlots[0]);

#ifdef _WIN32
	cleanup_win(&_nativeContext.window, &_nativeContext.windowDevice_Ctx, &_nativeContext.GL_ctx);
#elif defined(__linux__)
	cleanup_linux(_nativeContext.display, _nativeContext.GL_ctx);
#endif
}


void Topl_Renderer_GL4::init(NATIVE_WINDOW hwnd){
	_nativeContext.window = hwnd;
#ifdef _WIN32
    init_win(&_nativeContext.window, &_nativeContext.windowDevice_Ctx, &_nativeContext.GL_ctx);
#elif defined(__linux__)
	init_linux(_nativeContext.GL_ctx, _nativeContext.display, _nativeContext.window_ptr);
#endif
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST); // make these customizable
	glDepthFunc(GL_LESS); // make these customizable

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenBuffers(GL4_BUFFER_MAX, &_bufferSlots[0]);
	glGenVertexArrays(GL4_VERTEX_ARRAY_MAX, &_vertexArraySlots[0]);
	glGenTextures(GL4_TEXTURE_BINDINGS_MAX, &_textureSlots[0]);

	glLineWidth(1.5f);
	glPointSize(3.0f);
}

void Topl_Renderer_GL4::clearView(){
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Topl_Renderer_GL4::build(const Topl_Scene* scene){
	build(scene, &_defaultCamera);
}

void Topl_Renderer_GL4::build(const Topl_Scene* scene, const Topl_Camera* camera){
	std::vector<uint8_t> blockBytes; // container for constant and uniform buffer updates

	// scene uniform block buffer generation
	if (_entryShader->genSceneBlock(scene, camera, &blockBytes)) {
		_renderCtx.buffers.push_back(Buffer_GL4(_bufferSlots[_bufferIndex])); 
		_bufferIndex++; // increments to next available slot
		glBindBuffer(GL_UNIFORM_BUFFER, _renderCtx.buffers.back().buffer);
		unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	for (unsigned g = 0; g < scene->getActorCount(); g++) { // Slot index will signify how many buffers exist
		unsigned rID = g + 1;
		actor_cptr actor = scene->getGeoActor(rID - 1); // ids begin at 1, conversion is required
		Geo_RenderObj* actor_renderObj = (Geo_RenderObj*)actor->getRenderObj();

		vertex_cptr actor_vData = actor_renderObj->getVertices();
		ui_cptr actor_iData = actor_renderObj->getIndices();

		// component block buffer generation
		if (_entryShader->genGeoBlock(actor, &blockBytes)) {
			_renderCtx.buffers.push_back(Buffer_GL4(rID, BUFF_Render_Block, _bufferSlots[_bufferIndex]));
			_bufferIndex++; // increments to next available slot
			glBindBuffer(GL_UNIFORM_BUFFER, _renderCtx.buffers.back().buffer);
			unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
			glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		// index creation
		if (actor_iData != nullptr) {
			_renderCtx.buffers.push_back(Buffer_GL4(rID, BUFF_Index_UI, _bufferSlots[_bufferIndex], actor_renderObj->getIndexCount()));
			_bufferIndex++; // increments to next available slot
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _renderCtx.buffers.back().buffer); // Gets the latest buffer for now
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, actor_renderObj->getIndexCount() * sizeof(unsigned), actor_iData, GL_STATIC_DRAW);
		} else {
			_renderCtx.buffers.push_back(Buffer_GL4(rID, BUFF_Index_UI, _bufferSlots[_bufferIndex], 0)); // 0 indicates empty buffer
			_bufferIndex++; // increments to next available slot
		}

		_renderCtx.buffers.push_back(Buffer_GL4(rID, BUFF_Vertex_Type, _bufferSlots[_bufferIndex], actor_renderObj->getVerticesCount()));
		_bufferIndex++; // increments to next available slot
		glBindBuffer(GL_ARRAY_BUFFER, _renderCtx.buffers.back().buffer); // Gets the latest buffer for now
		glBufferData(GL_ARRAY_BUFFER, actor_renderObj->getVerticesCount() * sizeof(Geo_Vertex), actor_vData, GL_STATIC_DRAW);

		_renderCtx.VAOs.push_back(VertexArray_GL4(rID, _vertexArraySlots[_vertexArrayIndex]));
		_vertexArrayIndex++; // increment to next available slot
		VertexArray_GL4* currentVAO_ptr = &_renderCtx.VAOs.back(); // Check to see if all parameters are valid
		glBindVertexArray(currentVAO_ptr->vao);

		GLsizei inputElementOffset = 0;
		for(unsigned i = 0; i < _entryShader->getInputCount(); i++){
			const Shader_Type* shaderType = _entryShader->getInputAtIndex(i);

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,
				_GL4::getSizeFromShaderVal(shaderType->type),
				_GL4::getFormatFromShaderVal(shaderType->type),
				GL_FALSE,
				sizeof(Geo_Vertex),
				(inputElementOffset != 0) ? GL4_BUFFER_OFFSET(inputElementOffset) : NULL
			);

			inputElementOffset += _GL4::getOffsetFromShaderVal(shaderType->type);
		}

#ifdef RASTERON_H
		// TODO: Add support for multiple textures
		const Rasteron_Image* baseTex = scene->getFirstTexture(actor->getName());
		if(baseTex != nullptr) assignTexture(baseTex, rID); // Add the method definition
#endif
		_renderIDs = rID; // Sets main graphics ID's to max value of rID
	}

	GLint blockCount; glGetProgramiv(_pipeline->shaderProg, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	if (blockCount == TOPL_SINGLE_BLOCK_COUNT) // Render uniforms supported
		glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
	else if (blockCount == TOPL_FULL_BLOCK_COUNT) { // Render and Scene uniforms supported
		glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
		glUniformBlockBinding(_pipeline->shaderProg, SCENE_BLOCK_INDEX, SCENE_BLOCK_BINDING);
	}
	else {
		_isSceneReady = false; // Error
		return;
	}

	_isSceneReady = true;
    return; // To be continued
}

#ifdef RASTERON_H

// EXPERIMENTAL SCREEN CAPTURE CODE!
Rasteron_Image* Topl_Renderer_GL4::frame(){
	// custom Image format creation
	Rasteron_Image* image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));

	image->width = TOPL_WIN_WIDTH;
	image->height = TOPL_WIN_HEIGHT;
	image->name = "framebuff"; // TODO: Make this incremental, i.e framebuff1 framebuff2

	image->data = (uint32_t*)malloc(image->width * image->height * sizeof(uint32_t));
	glReadPixels(0, 0, image->width, image->height, GL_RGBA, GL_UNSIGNED_BYTE, image->data);

	return image;
}

void Topl_Renderer_GL4::assignTexture(const Rasteron_Image* image, unsigned id){
	// TODO: Check for format compatablitiy before this call, TEX_2D
	GLuint texture = _textureSlots[_textureIndex];
	_textureIndex++; // increments to next available slot
	glBindTexture(GL_TEXTURE_2D, texture);

	_GL4::setTextureProperties(GL_TEXTURE_2D, TEX_Wrap); // Setting this here

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->height, image->width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	_renderCtx.textures.push_back(Texture_GL4(id, TEX_2D, TEX_Wrap, texture));
}

#endif

void Topl_Renderer_GL4::update(const Topl_Scene* scene){
	update(scene, &_defaultCamera);
}

void Topl_Renderer_GL4::update(const Topl_Scene* scene, const Topl_Camera* camera){
	std::vector<uint8_t> blockBytes;
	Buffer_GL4* targetBuff = nullptr;

	if (_entryShader->genSceneBlock(scene, camera, &blockBytes) && _renderCtx.buffers.front().targetID == SPECIAL_SCENE_RENDER_ID) {
		glBindBuffer(GL_UNIFORM_BUFFER, _renderCtx.buffers.front().buffer);
		unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
	}

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		unsigned rID = g + 1;
		actor_cptr actor = scene->getGeoActor(rID - 1); // ids begin at 1, conversion is required
		if (_entryShader->genGeoBlock(actor, &blockBytes)) {
			for (std::vector<Buffer_GL4>::iterator currentBuff = _renderCtx.buffers.begin(); currentBuff < _renderCtx.buffers.end(); currentBuff++)
				if (currentBuff->targetID == rID && currentBuff->type == BUFF_Render_Block) targetBuff = &(*currentBuff);

			if (targetBuff == nullptr) puts("Block buffer could not be located! ");
			else {
				glBindBuffer(GL_UNIFORM_BUFFER, targetBuff->buffer);
				unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
				glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
			}
		}
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	_isSceneReady = true;
	return;
}

void Topl_Renderer_GL4::render(void){
	GLenum drawType;
	switch(_drawType){
	case DRAW_Triangles: drawType = GL_TRIANGLES; break;
	case DRAW_Points: drawType = GL_POINTS; break;
	case DRAW_Lines: drawType = GL_LINES; break;
	case DRAW_Fan: drawType = GL_TRIANGLE_FAN; break;
	case DRAW_Strip: drawType = GL_TRIANGLE_STRIP; break;
	default:
		puts("Draw type not supported yet!");
		return;
	}

	if (_renderCtx.buffers.front().targetID == SPECIAL_SCENE_RENDER_ID)
		glBindBufferBase(GL_UNIFORM_BUFFER, SCENE_BLOCK_BINDING, _renderCtx.buffers.front().buffer);

	Buffer_GL4** buffer_ptrs = (Buffer_GL4**)malloc(MAX_BUFFERS_PER_TARGET * sizeof(Buffer_GL4*));

	// Rendering Loop!
	if (_renderIDs == 1) return; // no rendering can occur without targets to draw
	else for (unsigned id = 1; id <= _renderIDs; id++) {
		for (std::vector<VertexArray_GL4>::iterator currentVAO = _renderCtx.VAOs.begin(); currentVAO < _renderCtx.VAOs.end(); currentVAO++)
			if (currentVAO->targetID == id) glBindVertexArray(currentVAO->vao);
			else continue; // if it continues all the way through error has occured

		// Buffer discovery and binding step
		_GL4::discoverBuffers(buffer_ptrs, &_renderCtx.buffers, id);

		Buffer_GL4* renderBlockBuff = _GL4::findBuffer(BUFF_Render_Block, buffer_ptrs, MAX_BUFFERS_PER_TARGET);
		if (renderBlockBuff != nullptr)
			glBindBufferBase(GL_UNIFORM_BUFFER, RENDER_BLOCK_BINDING, renderBlockBuff->buffer);

		Buffer_GL4* vertexBuff = _GL4::findBuffer(BUFF_Vertex_Type, buffer_ptrs, MAX_BUFFERS_PER_TARGET);
		Buffer_GL4* indexBuff = _GL4::findBuffer(BUFF_Index_UI, buffer_ptrs, MAX_BUFFERS_PER_TARGET);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuff->buffer);
		if(indexBuff != nullptr) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff->buffer);

		for (unsigned t = 0; t < _renderCtx.textures.size(); t++) {
			if (_renderCtx.textures.at(t).targetID > id) break; // This means we have passed it in sequence
			else if (_renderCtx.textures.at(t).targetID == id) {
				glBindTexture(GL_TEXTURE_2D, _renderCtx.textures.at(t).texture);
				break;
			}
		}

		// Drawing Call!
		if (indexBuff != nullptr && indexBuff->count != 0) glDrawElements(drawType, indexBuff->count, GL_UNSIGNED_INT, (void*)0);
		else glDrawArrays(drawType, 0, vertexBuff->count); // When no indices are present

		// Unbinding
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	free(buffer_ptrs);
#ifdef _WIN32 // Swap buffers in windows
	swapBuffers_win(&_nativeContext.windowDevice_Ctx);
#elif defined(__linux__)
	swapBuffers_linux(_nativeContext.display, _nativeContext.window_ptr);
#endif
}
