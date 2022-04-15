#include "opengl/Topl_Renderer_GL4.hpp"

namespace Renderer {
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
		default: logMessage(MESSAGE_Exclaim, "GL4 Shader Input Type Not Supported!"); break;
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
			logMessage(MESSAGE_Exclaim, "GL4 Type Shader Input Type Not Supported!");
			break;
		}
		return offset;
	}

	static Buffer_GL4* findBuff(Buffer_GL4** buffers, enum BUFF_Type type) {
		return *(buffers + type); // type arguments indicates the offset
	}

	static void discoverBuffers(Buffer_GL4** buffers, std::vector<Buffer_GL4>* targetBuffs, unsigned id) {
		for (std::vector<Buffer_GL4>::iterator buff = targetBuffs->begin(); buff < targetBuffs->end(); buff++)
			if (buff->targetID == id)
				*(buffers + buff->type) = &(*buff); // type arguments indicates the offset
	}

	static void setTextureProperties(GLenum type, TEX_Mode m) {
		switch (m) {
		case TEX_Wrap:
			glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		case TEX_Mirror:
			glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			break;
		case TEX_Clamp:
			glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		}
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

}

#ifdef _WIN32
static void init_win(const HWND* window, HDC* windowDC, HGLRC* hglrc){
    // Creates an HDC based on the window
    *(windowDC) = GetDC(*(window));

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

static void cleanup_win(HWND* window, HDC* windowDC, HGLRC* hglrc){
  	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(*(hglrc));

	ReleaseDC(*(window), *(windowDC));
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
	cleanup_win(&_platformCtx.window, &_platformCtx.windowDevice_Ctx, &_platformCtx.GL_ctx);
#elif defined(__linux__)
	cleanup_linux(_platformCtx.display, _platformCtx.GL_ctx);
#endif

	for (unsigned r = 0; r < _renderCtxIndex; r++) 
		delete(*(_renderCtx_GL4 + r)); // delete individual render contexts
	free(_renderCtx_GL4); // free the render context heap
}


void Topl_Renderer_GL4::init(NATIVE_WINDOW window){
	_platformCtx.window = window;
#ifdef _WIN32
    init_win(&_platformCtx.window, &_platformCtx.windowDevice_Ctx, &_platformCtx.GL_ctx);
#elif defined(__linux__)
	init_linux(_platformCtx.GL_ctx, _platformCtx.display, &_platformCtx.window);
#endif
	glewExperimental = GL_TRUE;
	glewInit();

	// Viewport Creation // add support for multiple!
	glViewport(0, 0, TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);

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
	glClearColor(CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_ALPHA);
	// glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Topl_Renderer_GL4::switchFramebuff(){
	if(_isSceneDrawn)
#ifdef _WIN32 // Swap buffers in windows
	swapBuffers_win(&_platformCtx.windowDevice_Ctx);
#elif defined(__linux__)
	swapBuffers_linux(_platformCtx.display, &_platformCtx.window);
#endif
	_isSceneDrawn = false; // awaiting another draw call
}

void Topl_Renderer_GL4::build(const Topl_Scene* scene){
	*(_renderCtx_GL4 + _renderCtxIndex) = new Topl_RenderContext_GL4(scene); // creation of new render context
	Topl_RenderContext_GL4* activeCtx = *(_renderCtx_GL4 + _renderCtxIndex);

	blockBytes_t blockBytes; // container for constant and uniform buffer updates
	// scene uniform block buffer generation
	if (_entryShader->genSceneBlock(scene, _activeCamera, &blockBytes)) {
		activeCtx->buffers.push_back(Buffer_GL4(_bufferSlots[_bufferIndex])); 
		_bufferIndex++; // increments to next available slot
		glBindBuffer(GL_UNIFORM_BUFFER, activeCtx->buffers.back().buffer);
		unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		unsigned rID = g + 1;
		actor_cptr actor = scene->getGeoActor(rID - 1); // ids begin at 1, conversion is required
		Geo_RenderObj* actor_renderObj = (Geo_RenderObj*)actor->getRenderObj();

		vertex_cptr_t actor_vData = actor_renderObj->getVertices();
		ui_cptr_t actor_iData = actor_renderObj->getIndices();

		// render block buffer generation
		if (_entryShader->genGeoBlock(actor, &blockBytes)) {
			activeCtx->buffers.push_back(Buffer_GL4(rID, BUFF_Render_Block, _bufferSlots[_bufferIndex]));
			_bufferIndex++; // increments to next available slot
			glBindBuffer(GL_UNIFORM_BUFFER, activeCtx->buffers.back().buffer);
			unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
			glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		// index creation
		if (actor_iData != nullptr) {
			activeCtx->buffers.push_back(Buffer_GL4(rID, BUFF_Index_UI, _bufferSlots[_bufferIndex], actor_renderObj->getIndexCount()));
			_bufferIndex++; // increments to next available slot
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, activeCtx->buffers.back().buffer); // Gets the latest buffer for now
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, actor_renderObj->getIndexCount() * sizeof(unsigned), actor_iData, GL_STATIC_DRAW);
		} else {
			activeCtx->buffers.push_back(Buffer_GL4(rID, BUFF_Index_UI, _bufferSlots[_bufferIndex], 0)); // 0 indicates empty buffer
			_bufferIndex++; // increments to next available slot
		}

		activeCtx->buffers.push_back(Buffer_GL4(rID, BUFF_Vertex_Type, _bufferSlots[_bufferIndex], actor_renderObj->getVertexCount()));
		_bufferIndex++; // increments to next available slot
		glBindBuffer(GL_ARRAY_BUFFER, activeCtx->buffers.back().buffer); // Gets the latest buffer for now
		glBufferData(GL_ARRAY_BUFFER, actor_renderObj->getVertexCount() * sizeof(Geo_Vertex), actor_vData, GL_STATIC_DRAW);

		activeCtx->VAOs.push_back(VertexArray_GL4(rID, _vertexArraySlots[_vertexArrayIndex]));
		_vertexArrayIndex++; // increment to next available slot
		VertexArray_GL4* currentVAO_ptr = &activeCtx->VAOs.back(); // Check to see if all parameters are valid
		glBindVertexArray(currentVAO_ptr->vao);

		GLsizei inputElementOffset = 0;
		for(unsigned i = 0; i < _entryShader->getInputCount(); i++){
			const Shader_Type* shaderType = _entryShader->getInputAtIndex(i);

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,
				Renderer::getSizeFromShaderVal(shaderType->type),
				Renderer::getFormatFromShaderVal(shaderType->type),
				GL_FALSE,
				sizeof(Geo_Vertex),
				(inputElementOffset != 0) ? GL4_BUFFER_OFFSET(inputElementOffset) : NULL
			);

			inputElementOffset += Renderer::getOffsetFromShaderVal(shaderType->type);
		}
		_renderIDs = rID; // Sets main graphics ID's to max value of rID
	}

	GLint blockCount; 
	glGetProgramiv(_pipeline->shaderProg, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	if (blockCount == RENDER_BLOCK_SUPPORT) // Render uniforms supported
		glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
	else if (blockCount == SCENE_BLOCK_SUPPORT) { // Render and Scene uniforms supported
		glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
		glUniformBlockBinding(_pipeline->shaderProg, SCENE_BLOCK_INDEX, SCENE_BLOCK_BINDING);
	}

	_isSceneReady = true;
}

#ifdef RASTERON_H

Rasteron_Image* Topl_Renderer_GL4::frame(){
	// TODO: Crop to viewport!
	// see https://stackoverflow.com/questions/29764925/how-to-get-set-the-width-and-height-of-the-default-framebuffer
	unsigned viewportHeight = Platform::getViewportHeight(_platformCtx.window);
	unsigned viewportWidth = Platform::getViewportWidth(_platformCtx.window);

	Rasteron_Image* rawImage = allocNewImg("framebuff", TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH);
	// Rasteron_Image* rawImage = allocNewImg("framebuff", viewportWidth, viewportHeight);
	glReadPixels(0, 0, rawImage->width, rawImage->height, GL_RGBA, GL_UNSIGNED_BYTE, rawImage->data);
	Rasteron_Image* flipImage = createImgFlip(rawImage, FLIP_Upside);
	// Rasteron_Image* image = allocNewImg("framebuff-crop", viewportHeight, viewportWidth);

	deleteImg(rawImage);
	// deleteImg(flipImage);
	return flipImage;
}

void Topl_Renderer_GL4::texturize(const Topl_Scene* scene) {
	Topl_RenderContext_GL4* activeCtx = getRenderContext(scene);
	if (activeCtx == nullptr) return logMessage(MESSAGE_Exclaim, "Render Context could not be found!");

#ifdef RASTERON_H // Rasteron dependency required for updating textures
	// Need to clear saved textures entirely for texture update
	activeCtx->textures.clear();
	glDeleteTextures(GL4_TEXTURE_BINDINGS_MAX, &_textureSlots[0]);
	_textureIndex = 0;

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		unsigned rID = g + 1;
		actor_cptr actor = scene->getGeoActor(rID - 1); // ids begin at 1, conversion is required

		// TODO: Add support for multiple textures
		const Rasteron_Image* baseTex = scene->getTexture(actor->getName());
		if (baseTex != nullptr) attachTexture(baseTex, rID);
	}
#endif
}

void Topl_Renderer_GL4::attachTexture(const Rasteron_Image* rawImage, unsigned id){
	// TODO: find id corresponding to proper render context
	// Topl_RenderContext_Drx11* activeCtx = getRenderContext(scene);
	Topl_RenderContext_GL4* activeCtx = *(_renderCtx_GL4); // for now gets the first available render context
	if (activeCtx == nullptr) return logMessage(MESSAGE_Exclaim, "Render Context could not be found!");
	
	GLuint texture = _textureSlots[_textureIndex];
	_textureIndex++; // increments to next available slot

	glBindTexture(GL_TEXTURE_2D, texture);

	Renderer::setTextureProperties(GL_TEXTURE_2D, _texMode); // setting texture mode properties
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rawImage->height, rawImage->width, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawImage->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	activeCtx->textures.push_back(Texture_GL4(id, TEX_2D, _texMode, texture));
}

void Topl_Renderer_GL4::attachMaterial(const Topl_Material* material, unsigned id) {
	// Implement Body
}

#endif

void Topl_Renderer_GL4::update(const Topl_Scene* scene){
	Topl_RenderContext_GL4* activeCtx = getRenderContext(scene);
	if (activeCtx == nullptr) return logMessage(MESSAGE_Exclaim, "Render Context could not be found!");

	blockBytes_t blockBytes;
	Buffer_GL4* targetBuff = nullptr;

	if (_entryShader->genSceneBlock(scene, _activeCamera, &blockBytes) && activeCtx->buffers.front().targetID == SPECIAL_SCENE_RENDER_ID) {
		glBindBuffer(GL_UNIFORM_BUFFER, activeCtx->buffers.front().buffer);
		unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
	}

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		unsigned rID = g + 1;
		actor_cptr actor = scene->getGeoActor(rID - 1); // ids begin at 1, conversion is required
		if (_entryShader->genGeoBlock(actor, &blockBytes)) {
			for (std::vector<Buffer_GL4>::iterator buff = activeCtx->buffers.begin(); buff < activeCtx->buffers.end(); buff++)
				if (buff->targetID == rID && buff->type == BUFF_Render_Block) targetBuff = &(*buff);

			if (targetBuff == nullptr) logMessage(MESSAGE_Exclaim, "Block buffer could not be located! ");
			else {
				glBindBuffer(GL_UNIFORM_BUFFER, targetBuff->buffer);
				unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
				glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
			}
		}
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return;
}

void Topl_Renderer_GL4::drawMode(){
	switch(_drawMode){
	case DRAW_Triangles: _drawMode_GL4 = GL_TRIANGLES; break;
	case DRAW_Points: _drawMode_GL4 = GL_POINTS; break;
	case DRAW_Lines: _drawMode_GL4 = GL_LINES; break;
	case DRAW_Fan: _drawMode_GL4 = GL_TRIANGLE_FAN; break;
	case DRAW_Strip: _drawMode_GL4 = GL_TRIANGLE_STRIP; break;
	default: return logMessage(MESSAGE_Exclaim, "Draw type not supported!");
	}
}

void Topl_Renderer_GL4::render(const Topl_Scene* scene){
	Topl_RenderContext_GL4* activeCtx = getRenderContext(scene);
	if (activeCtx == nullptr) return logMessage(MESSAGE_Exclaim, "Render Context could not be found!");

	if (activeCtx->buffers.front().targetID == SPECIAL_SCENE_RENDER_ID)
		glBindBufferBase(GL_UNIFORM_BUFFER, SCENE_BLOCK_BINDING, activeCtx->buffers.front().buffer);

	Buffer_GL4** buffers = (Buffer_GL4**)malloc(BUFFERS_PER_RENDERTARGET * sizeof(Buffer_GL4*));

	// Rendering Loop!
	for (unsigned id = 1; id <= _renderIDs; id++) {
		for (std::vector<VertexArray_GL4>::iterator currentVAO = activeCtx->VAOs.begin(); currentVAO < activeCtx->VAOs.end(); currentVAO++)
			if (currentVAO->targetID == id) glBindVertexArray(currentVAO->vao);
			else continue; // if it continues all the way through error has occured

		// Buffer discovery and binding step
		Renderer::discoverBuffers(buffers, &activeCtx->buffers, id);

		Buffer_GL4* renderBlockBuff = Renderer::findBuff(buffers, BUFF_Render_Block);
		if (renderBlockBuff != nullptr)
			glBindBufferBase(GL_UNIFORM_BUFFER, RENDER_BLOCK_BINDING, renderBlockBuff->buffer);

		Buffer_GL4* vertexBuff = Renderer::findBuff(buffers, BUFF_Vertex_Type);
		Buffer_GL4* indexBuff = Renderer::findBuff(buffers, BUFF_Index_UI);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuff->buffer);
		if(indexBuff != nullptr) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff->buffer);

		for (unsigned t = 0; t < activeCtx->textures.size(); t++) {
			if (activeCtx->textures.at(t).targetID > id) break; // Geometry actor is passed in sequence 
			else if (activeCtx->textures.at(t).targetID == id) {
				glBindTexture(GL_TEXTURE_2D, activeCtx->textures.at(t).texture);
				break;
			}
		}

		// Drawing Call!
		if (indexBuff != nullptr && indexBuff->count != 0) glDrawElements(_drawMode_GL4, indexBuff->count, GL_UNSIGNED_INT, (void*)0);
		else glDrawArrays(_drawMode_GL4, 0, vertexBuff->count); // When no indices are present

		// Unbinding
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	free(buffers);

	_isSceneDrawn = true;
}