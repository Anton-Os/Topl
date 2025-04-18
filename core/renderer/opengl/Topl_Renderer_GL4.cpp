#include "renderer/opengl/Topl_Renderer_GL4.hpp"

namespace GL4 {
	GLenum getShaderFormat(enum SHDR_ValueType type) {
		switch (type) {
		case SHDR_float_vec4: case SHDR_float_vec3: case SHDR_float_vec2: case SHDR_float:
			return GL_FLOAT;
		case SHDR_uint_vec4: case SHDR_uint_vec3: case SHDR_uint_vec2: case SHDR_uint:
			return GL_UNSIGNED_INT;
		default:
			logMessage(MESSAGE_Exclaim, "GL4 Shader Input Type Not Supported!");
			return 0; // error
		}
	}

	void genVertexArrayLayout(GL4::VertexArray* VAO, entry_shader_cptr entryShader) {
		glBindVertexArray(VAO->vao);

		GLsizei inputElementOffset = 0;
		for (unsigned inputNum = 0; inputNum < entryShader->getInputCount(); inputNum++) {
			const Shader_Type* shaderType = entryShader->getInputAtIndex(inputNum);

			glEnableVertexAttribArray(inputNum);
			glVertexAttribPointer(
				inputNum,
				(GLint)Topl_Pipeline::getSize(shaderType->type),
				getShaderFormat(shaderType->type),
				GL_FALSE,
				sizeof(Geo_Vertex),
				(inputElementOffset != 0) ? reinterpret_cast<void*>(inputElementOffset) : NULL // This line causes issus?
			);

			inputElementOffset += Topl_Pipeline::getOffset(shaderType->type);
		}

		glBindVertexArray(0);
	}

	void setTextureProperties(GLenum type, TEX_Mode m) {
		switch (m) {
		case TEX_Wrap:
			glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_REPEAT);
			break;
		case TEX_Mirror:
			glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
			break;
		case TEX_Clamp:
			glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			break;
		}
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

#ifdef _WIN32
static void cleanup_win(HWND* window, HDC* windowDC, HGLRC* hglrc) {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(*hglrc);

	ReleaseDC(*window, *windowDC);
}
#elif defined(__linux__) && !defined(__ANDROID__)
static void cleanup_linux(Display* display, GLXContext graphicsContext) { glXDestroyContext(display, graphicsContext); }
#endif

Topl_Renderer_GL4::~Topl_Renderer_GL4() {
	glDeleteBuffers(MAX_RENDERID, _bufferSlots);
	glDeleteBuffers(MAX_RENDERID, _storageSlots);
	glDeleteVertexArrays(MAX_RENDERID, _vertexArraySlots);
	glDeleteTextures(MAX_RENDERID, _textureSlots);

#ifdef _WIN32
	cleanup_win(&_platformCtx->window, &_platformCtx->deviceCtx, &_platformCtx->oglCtx);
#elif defined(__linux__) && !defined(__ANDROID__)
	cleanup_linux(_platformCtx->display, _platformCtx->oglCtx);
#endif
}

#ifdef _WIN32
static void init_win(const HWND* window, HDC* windowDC, HGLRC* hglrc) {
	// Creates an HDC based on the window
	*windowDC = GetDC(*window);

	// Pixel Format for Windows GL Context
	PIXELFORMATDESCRIPTOR pixFrmtDesc, * pixFrmtDesc_ptr;
	pixFrmtDesc_ptr = &pixFrmtDesc;
	int pixFrmt;

	pixFrmtDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixFrmtDesc.nVersion = 1;
	pixFrmtDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixFrmtDesc.dwLayerMask = PFD_MAIN_PLANE;
	pixFrmtDesc.iPixelType = PFD_TYPE_RGBA;
	pixFrmtDesc.cColorBits = 24;

	pixFrmt = ChoosePixelFormat(*windowDC, pixFrmtDesc_ptr);
	BOOL pixFrmtChk = SetPixelFormat(*windowDC, pixFrmt, pixFrmtDesc_ptr);

	// wgl initialization functions
	*hglrc = wglCreateContext(*windowDC);
	wglMakeCurrent(*windowDC, *hglrc);
}
#elif defined(__linux__) && !defined(__ANDROID__)
static void init_linux(Display* display, Window* window, XVisualInfo* visualInfo, GLXContext* oglCtx) {
	int major, minor;
	if(glXQueryVersion(display, &major, &minor)) std::cout << "GLX Version is " << std::to_string(major) << "." << std::to_string(minor) << std::endl;
	else logMessage(MESSAGE_Exclaim, "Issue with GLX Version query");

	/* GLint visualInfoAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	 *   XVisualInfo* visualInfo = glXChooseVisual(display, 0, visualInfoAttribs);
	 *   if(visualInfo == nullptr) return logMessage(MESSAGE_Exclaim, "Issue with GLX Visual"); */

	*oglCtx = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
	glXMakeCurrent(display, *window, *oglCtx);
}
#endif

void Topl_Renderer_GL4::init(NATIVE_WINDOW window) {
	_platformCtx->window = window;

#ifdef _WIN32
    init_win(&_platformCtx->window, &_platformCtx->deviceCtx, &_platformCtx->oglCtx);
#elif defined(__linux__) && !defined(__ANDROID__)
    init_linux(_platformCtx->display, &_platformCtx->window, _platformCtx->visualInfo, &_platformCtx->oglCtx);
#endif
#ifndef __ANDROID__
    glewExperimental = GL_TRUE;
    glewInit();
#endif
	glEnable(GL_DEPTH_TEST); // make these customizable
    glDepthFunc(GL_LESS); // make these customizable

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _bufferSlots = (GLuint*)malloc(MAX_RENDERID * sizeof(GLuint));
    glGenBuffers(MAX_RENDERID, _bufferSlots);
    _textureSlots = (GLuint*)malloc(MAX_RENDERID * sizeof(GLuint));
    glGenTextures(MAX_RENDERID, _textureSlots);
	_storageSlots = (GLuint*)malloc(MAX_RENDERID * sizeof(GLuint));
	glGenBuffers(MAX_RENDERID, _storageSlots);
#ifndef __ANDROID__
    _vertexArraySlots = (GLuint*)malloc(MAX_RENDERID * sizeof(GLuint)); // vertex arrays not supported EGL
    glGenVertexArrays(MAX_RENDERID, _vertexArraySlots);

    // glDisable(GL_CULL_FACE);
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    glLineWidth(1.5f);
    glPointSize(3.0f);
#endif
}

void Topl_Renderer_GL4::clear() {
	glClearColor(_clearColors[0], _clearColors[1], _clearColors[2], _clearColors[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Topl_Renderer_GL4::setViewport(const Topl_Viewport* viewport) {
	// TODO: Scale to viewport
    unsigned width = Platform::getViewportWidth(_platformCtx);
    unsigned height = Platform::getViewportHeight(_platformCtx);

	if (viewport != nullptr) glViewport(viewport->xOffset, viewport->yOffset, height, width);
	else _flags[BUILD_BIT] = false; // Error
}

void Topl_Renderer_GL4::swapBuffers(double frameTime) {
#ifdef _WIN32
		SwapBuffers(_platformCtx->deviceCtx);
#elif defined(__linux__) && !defined(__ANDROID__)
		glXSwapBuffers(_platformCtx->display, _platformCtx->window);
#endif
}

void Topl_Renderer_GL4::build(const Geo_Actor* actor){
	if(actor == SCENE_RENDERID){
		_blockBufferMap.insert({ SCENE_RENDERID, GL4::Buffer(*(_bufferSlots + _bufferIndex)) });
		_bufferIndex++; // increments to next available slot
		glBindBuffer(GL_UNIFORM_BUFFER, _blockBufferMap.at(SCENE_RENDERID).buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uint8_t) * _sceneBlockData.size(), _sceneBlockData.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		_storageBufferMap.insert({ SCENE_RENDERID, GL4::Buffer(*(_storageSlots + _storageIndex)) });
		_storageIndex;
	} else {
		unsigned long renderID = getRenderID(actor);
		Geo_Mesh* mesh = (Geo_Mesh*)actor->getMesh();

		// render buffer
		if(_blockBufferMap.find(renderID) == _blockBufferMap.end()){
			_blockBufferMap.insert({ renderID, GL4::Buffer(renderID, BUFF_Render_Block, *(_bufferSlots + _bufferIndex)) });
			_bufferIndex++; // increments to next available slot
		}
		glBindBuffer(GL_UNIFORM_BUFFER, _blockBufferMap.at(renderID).buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uint8_t) * _actorBlockData.size(), _actorBlockData.data(), GL_STATIC_DRAW);
		
		// mesh buffer
		if(_extBlockBufferMap.find(renderID) == _extBlockBufferMap.end()){
			_extBlockBufferMap.insert({ renderID, GL4::Buffer(renderID, BUFF_Ext_Block, *(_bufferSlots + _bufferIndex)) });
			_bufferIndex++; // increments to next available slot
		}
		glBindBuffer(GL_UNIFORM_BUFFER, _extBlockBufferMap.at(renderID).buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uint8_t) * _meshBlockData.size(), _meshBlockData.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// storage buffer
		if(_storageBufferMap.find(renderID) == _storageBufferMap.end()){
			_storageBufferMap.insert({ renderID, GL4::Buffer(*(_storageSlots + _storageIndex)) });
			_storageIndex++;
		}
		glBindBuffer(GL_SHADER_STORAGE_BLOCK, _storageBufferMap.at(renderID).buffer);
		// TODO: Populate data from vertex positions
		glBindBuffer(GL_SHADER_STORAGE_BLOCK, 0);

		// indices generation
		if(_indexBufferMap.find(renderID) == _indexBufferMap.end()){
			(mesh->getIndices() != nullptr)
				? _indexBufferMap.insert({ renderID, GL4::Buffer(renderID, BUFF_Index_UI, *(_bufferSlots + _bufferIndex), mesh->getIndexCount()) })
				: _indexBufferMap.insert({ renderID, GL4::Buffer(renderID, BUFF_Index_UI, *(_bufferSlots + _bufferIndex), 0) });
			_bufferIndex++; // increments to next available slot
		}
		if (mesh->getIndices() != nullptr) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferMap.at(renderID).buffer); // gets the latest buffer
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndexCount() * sizeof(unsigned), mesh->getIndices(), GL_STATIC_DRAW);
		}

		// vertices generation
		if(_vertexBufferMap.find(renderID) == _vertexBufferMap.end()){
			_vertexBufferMap.insert({ renderID, GL4::Buffer(renderID, BUFF_Vertex_Type, *(_bufferSlots + _bufferIndex), mesh->getVertexCount()) });
			_bufferIndex++; // increments to next available slot
		}
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferMap.at(renderID).buffer); // gets the latest buffer
		glBufferData(GL_ARRAY_BUFFER, mesh->getVertexCount() * sizeof(Geo_Vertex), mesh->getVertices(), GL_STATIC_DRAW);

		// setting vertex input layout
		if(_vertexArrayMap.find(renderID) == _vertexArrayMap.end()){
			_vertexArrayMap.insert({ renderID, GL4::VertexArray(renderID, *(_vertexArraySlots + _vertexArrayIndex)) });
			_vertexArrayIndex++; // increment to next available slot
			GL4::genVertexArrayLayout(&_vertexArrayMap.at(renderID), _entryShader);
		}

		// TODO: Populate storage buffer

		_flags[BUILD_BIT] = true;
	}
}

void Topl_Renderer_GL4::update(const Geo_Actor* actor){
	if(actor == SCENE_RENDERID){
		glBindBuffer(GL_UNIFORM_BUFFER, _blockBufferMap.at(SCENE_RENDERID).buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uint8_t) * _sceneBlockData.size(), _sceneBlockData.data(), GL_STATIC_DRAW);
	} else {
		unsigned renderID = getRenderID(actor);
		
		glBindBuffer(GL_UNIFORM_BUFFER, _blockBufferMap.at(renderID).buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uint8_t) * _actorBlockData.size(), _actorBlockData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, _extBlockBufferMap.at(renderID).buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uint8_t) * _meshBlockData.size(), _meshBlockData.data(), GL_STATIC_DRAW);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Topl_Renderer_GL4::setDrawMode(enum DRAW_Mode mode) {
	_drawMode = mode;

	switch (_drawMode) {
	case DRAW_Triangles: _drawMode_GL4 = GL_TRIANGLES; break;
	case DRAW_Points: _drawMode_GL4 = GL_POINTS; break;
	case DRAW_Lines: _drawMode_GL4 = GL_LINES; break;
	case DRAW_Fan: _drawMode_GL4 = GL_TRIANGLE_FAN; break;
	case DRAW_Strip: _drawMode_GL4 = GL_TRIANGLE_STRIP; break;
#ifndef __ANDROID__
	case DRAW_Patch: _drawMode_GL4 = GL_PATCHES; break;
#endif
	default: break;
	}
}

void Topl_Renderer_GL4::draw(const Geo_Actor* actor) {
	unsigned long renderID = _renderTargetMap[actor];
	// static GL4::Buffer *sceneBlockBuff, *renderBlockBuff, *vertexBuff, *indexBuff;
	if (renderID == SCENE_RENDERID && _blockBufferMap.at(SCENE_RENDERID).renderID == SCENE_RENDERID){ // Scene Target
		glBindBufferBase(GL_UNIFORM_BUFFER, SCENE_BLOCK_BINDING, _blockBufferMap.at(SCENE_RENDERID).buffer);
		if(_storageBufferMap.find(SCENE_BLOCK_BINDING) != _storageBufferMap.end()) glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SCENE_BLOCK_BINDING, _storageBufferMap.at(SCENE_RENDERID).buffer);
		for(unsigned b = 0; b < MAX_TEX_BINDINGS - 1; b++){
			auto tex2D = std::find_if(_textures.begin(), _textures.end(), [renderID, b](const GL4::Texture& t){ return t.renderID == renderID && t.format == TEX_2D && t.binding == b + 1; });
			if (tex2D != _textures.end()){
				glActiveTexture(GL_TEXTURE0 + tex2D->binding);
				glBindTexture(GL_TEXTURE_2D, tex2D->texture);
			}
		}
	}
	else if (actor->getMesh() != nullptr && actor->isShown) { // Drawable Target
		if(actor->getMesh()->drawMode != DRAW_Default) setDrawMode(actor->getMesh()->drawMode);
		// Data & Buffer Updates
		
		if(_vertexArrayMap.find(renderID) != _vertexArrayMap.end()) glBindVertexArray(_vertexArrayMap.at(renderID).vao);

		if(_vertexBufferMap.find(renderID) != _vertexBufferMap.end()) glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferMap.at(renderID).buffer);
		if(_indexBufferMap.find(renderID) != _indexBufferMap.end()) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferMap.at(renderID).buffer);
		if(_blockBufferMap.find(renderID) != _blockBufferMap.end()) glBindBufferBase(GL_UNIFORM_BUFFER, RENDER_BLOCK_BINDING, _blockBufferMap.at(renderID).buffer);
		if(_extBlockBufferMap.find(renderID) != _extBlockBufferMap.end()) glBindBufferBase(GL_UNIFORM_BUFFER, EXT_BLOCK_BINDING, _extBlockBufferMap.at(renderID).buffer);
		if(_storageBufferMap.find(renderID) != _storageBufferMap.end()) glBindBufferBase(GL_SHADER_STORAGE_BLOCK, RENDER_BLOCK_BINDING, _storageBufferMap.at(renderID).buffer);

		// Texture Updates 
	
		auto tex2D = std::find_if(_textures.begin(), _textures.end(), [renderID](const GL4::Texture& t){ return t.renderID == renderID && t.format == TEX_2D && t.binding == 0; });
		if (tex2D != _textures.end()){
			glActiveTexture(GL_TEXTURE0 + tex2D->binding);
			glBindTexture(GL_TEXTURE_2D, tex2D->texture);
			if(tex2D->mode != _texMode){ // setting proper tex mode
				setTexMode(tex2D->mode);
				GL4::setTextureProperties(GL_TEXTURE_2D, _texMode); 
			}
		}
		auto tex3D = std::find_if(_textures.begin(), _textures.end(), [renderID](const GL4::Texture& t){ return t.renderID == renderID && t.format == TEX_3D; });
		if(tex3D != _textures.end()){
			glActiveTexture(GL_TEXTURE0 + MAX_TEX_BINDINGS);
			glBindTexture(GL_TEXTURE_3D, tex3D->texture);
			if(tex3D->mode != _texMode){ // setting proper tex mode
				setTexMode(tex3D->mode);
				GL4::setTextureProperties(GL_TEXTURE_2D, _texMode); 
			}
		}

		// Draw Call!
        if (_vertexBufferMap.find(renderID) != _vertexBufferMap.end()){
			unsigned drawCount = (actor->getMesh()->drawMax != 0)? actor->getMesh()->drawMax : 0;
			if(drawCount == 0) drawCount = (_indexBufferMap.find(renderID) != _indexBufferMap.end())? _indexBufferMap.at(renderID).count : _vertexBufferMap.at(renderID).count;

            if(actor->getMesh()->getInstanceCount() <= 1){
                if (_indexBufferMap.find(renderID) != _indexBufferMap.end()) glDrawElements(_drawMode_GL4, drawCount, GL_UNSIGNED_INT, (void*)actor->getMesh()->drawMin); // indexed draw
                else glDrawArrays(_drawMode_GL4, actor->getMesh()->drawMin, drawCount); // non-indexed draw
            } else {
                if (_indexBufferMap.find(renderID) != _indexBufferMap.end()) glDrawElementsInstanced(_drawMode_GL4, drawCount, GL_UNSIGNED_INT, (void*)actor->getMesh()->drawMin, actor->getMesh()->getInstanceCount()); // instanced indexed draw
                else glDrawArraysInstanced(_drawMode_GL4, actor->getMesh()->drawMin, drawCount, actor->getMesh()->getInstanceCount()); // instanced non-indexed draw
            }
        } else logMessage(MESSAGE_Exclaim, "Corrupt Vertex Buffer!");

		// TODO: Conditionally restore previous draw mode

		// Unbinding
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

#ifdef RASTERON_H

Sampler_2D Topl_Renderer_GL4::frame() {
    unsigned viewportHeight = Platform::getViewportHeight(_platformCtx);
    unsigned viewportWidth = Platform::getViewportWidth(_platformCtx);

	Rasteron_Image* stageImage = RASTERON_ALLOC("frame", viewportWidth, viewportHeight);
	glReadPixels(0, 0, viewportHeight, viewportWidth, GL_RGBA, GL_UNSIGNED_BYTE, stageImage->data);

	Rasteron_Image* flipImage = flipImgOp(stageImage, FLIP_Upside); // flipping image over
	for(unsigned p = 0; p < flipImage->width * flipImage->height; p++) *(flipImage->data + p) = swap_rb(*(flipImage->data + p));

	_frameImage = Sampler_2D(mirrorImgOp(flipImage));

	RASTERON_DEALLOC(stageImage); RASTERON_DEALLOC(flipImage);
	return _frameImage;
}

void Topl_Renderer_GL4::attachTexAt(const Sampler_2D* image, unsigned renderID, unsigned binding) {
	GLuint textureTarget = *(_textureSlots + _textureIndex);
	_textureIndex++; // increments to next available slot

	for (std::vector<GL4::Texture>::iterator tex = _textures.begin(); tex != _textures.end(); tex++) {
		if (tex->renderID == renderID && tex->binding == binding && tex->format == TEX_2D) { // multi-texture substitution
			textureTarget = tex->texture;
			if (_textureIndex > 1) _textureIndex--; // decrement if texture binding is located
		}
	}

	glActiveTexture(GL_TEXTURE0 + binding);
	glBindTexture(GL_TEXTURE_2D, textureTarget);

    GL4::setTextureProperties(GL_TEXTURE_2D, image->mode);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->getImage()->height, image->getImage()->width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->getImage()->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (*(_textureSlots + _textureIndex - 1) == textureTarget)
        _textures.push_back(GL4::Texture(renderID, (unsigned short)binding, TEX_2D, image->mode, textureTarget)); // multi-texture addition
}

void Topl_Renderer_GL4::attachTex3D(const Sampler_3D* volumeTex, unsigned renderID) {
	GLuint textureTarget = *(_textureSlots + _textureIndex);
	_textureIndex++; // increments to next available slot

	for (std::vector<GL4::Texture>::iterator tex = _textures.begin(); tex != _textures.end(); tex++) {
		if (tex->renderID == renderID && tex->format == TEX_3D) {
			textureTarget = tex->texture;
			if (_textureIndex > 1) _textureIndex--; // decrement since texture binding is located
		}
	}

	glActiveTexture(GL_TEXTURE0 + MAX_TEX_BINDINGS);
	glBindTexture(GL_TEXTURE_3D, textureTarget);

	const Sampler_2D* volumeTexImage = volumeTex->getVolumeImg();
    GL4::setTextureProperties(GL_TEXTURE_3D, volumeTex->mode);
	glTexImage3D(
		GL_TEXTURE_3D,
		0, GL_RGBA,
		volumeTex->getWidth(),
		volumeTex->getHeight(),
		volumeTex->getDepth(),
		0, GL_RGBA,
		GL_UNSIGNED_BYTE,
		volumeTexImage->getImage()->data
	);
	glGenerateMipmap(GL_TEXTURE_3D);

	if (*(_textureSlots + _textureIndex - 1) == textureTarget)
        _textures.push_back(GL4::Texture(renderID, TEX_3D, volumeTex->mode, textureTarget));
}

#endif
