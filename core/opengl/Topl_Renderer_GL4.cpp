#include "opengl/Topl_Renderer_GL4.hpp"

namespace GL4 {
	// Shader Functions

	static GLenum getShaderFormat(enum SHDR_ValueType type) {
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

	static void genVertexArrayLayout(VertexArray_GL4* VAO, entry_shader_cptr entryShader) {
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
				(inputElementOffset != 0) ? (void*)(inputElementOffset) : NULL
			);

			inputElementOffset += Topl_Pipeline::getOffset(shaderType->type);
		}

		glBindVertexArray(0);
	}

	// Additional Funtions

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
static void cleanup_win(HWND* window, HDC* windowDC, HGLRC* hglrc) {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(*hglrc);

	ReleaseDC(*window, *windowDC);
}
#elif defined(__linux__)
static void cleanup_linux(Display* display, GLXContext graphicsContext) { glXDestroyContext(display, graphicsContext); }
#endif

Topl_Renderer_GL4::~Topl_Renderer_GL4() {
	glDeleteBuffers(GL4_BUFFER_MAX, &_bufferSlots[0]);
	glDeleteVertexArrays(GL4_VERTEX_ARRAY_MAX, &_vertexArraySlots[0]);
	glDeleteTextures(GL4_TEXTURE_BINDINGS_MAX, &_textureSlots[0]);

#ifdef _WIN32
	cleanup_win(&_platformCtx.window, &_platformCtx.deviceCtx, &_platformCtx.oglCtx);
#elif defined(__linux__)
	cleanup_linux(_platformCtx.display, _platformCtx.oglCtx);
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

static inline void swapBuffers_win(HDC* windowDC) { SwapBuffers(*windowDC); }

#elif defined(__linux__)
static void init_linux(GLXContext graphicsContext, Display* display, Window* window) {
	GLint visualInfoAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo* visualInfo = glXChooseVisual(display, 0, visualInfoAttribs);

	graphicsContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
	glXMakeCurrent(display, *window, graphicsContext);
}

static void swapBuffers_linux(Display* display, Window* window) { glXSwapBuffers(display, *window); }
#endif

void Topl_Renderer_GL4::init(NATIVE_WINDOW window) {
	_platformCtx.window = window;

#ifdef _WIN32
	init_win(&_platformCtx.window, &_platformCtx.deviceCtx, &_platformCtx.oglCtx);
#elif defined(__linux__)
	init_linux(_platformCtx.oglCtx, _platformCtx.display, &_platformCtx.window);
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

	setViewport(&_defaultViewport);
}

void Topl_Renderer_GL4::clearView() {
	glClearColor(CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_ALPHA);
	// glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Topl_Renderer_GL4::setViewport(const Topl_Viewport* viewport) {
	if (viewport != nullptr) glViewport(viewport->xOffset, viewport->yOffset, viewport->width, viewport->height);
	else _isBuilt = false; // Error
}

void Topl_Renderer_GL4::switchFramebuff() {
	if (_isDrawn) {
#ifdef _WIN32 // Swap buffers in windows
		swapBuffers_win(&_platformCtx.deviceCtx);
#elif defined(__linux__)
		swapBuffers_linux(_platformCtx.display, &_platformCtx.window);
#endif
		_isDrawn = false; // await future draw call
	}
}

void Topl_Renderer_GL4::build(const Topl_Scene* scene) {
	blockBytes_t shaderBlockData;

	// scene block buffer generation
	_entryShader->genSceneBlock(scene, _activeCamera, &shaderBlockData);
	if (!shaderBlockData.empty()) {
		_buffers.push_back(Buffer_GL4(_bufferSlots[_bufferIndex]));
		_bufferIndex++; // increments to next available slot
		glBindBuffer(GL_UNIFORM_BUFFER, _buffers.back().buffer);
		unsigned blockSize = sizeof(uint8_t) * shaderBlockData.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, shaderBlockData.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		_renderIDs++;
		_renderTargets_map.insert({ _renderIDs, scene->getGeoActor(g) });
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);
		// Geo_RenderObj* renderObj = (Geo_RenderObj*)actor->getRenderObj();
		Geo_Renderable* renderObj = (Geo_Renderable*)actor->getRenderable();

		// render block buffer generation
		if (!shaderBlockData.empty()) {
			_entryShader->genRenderBlock(actor, renderID, &shaderBlockData);
			_buffers.push_back(Buffer_GL4(renderID, BUFF_Render_Block, _bufferSlots[_bufferIndex]));
			_bufferIndex++; // increments to next available slot
			glBindBuffer(GL_UNIFORM_BUFFER, _buffers.back().buffer);
			unsigned blockSize = sizeof(uint8_t) * shaderBlockData.size();
			glBufferData(GL_UNIFORM_BUFFER, blockSize, shaderBlockData.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		// indices generation
		if (renderObj->getIndices() != nullptr) {
			_buffers.push_back(Buffer_GL4(renderID, BUFF_Index_UI, _bufferSlots[_bufferIndex], renderObj->getIndexCount()));
			_bufferIndex++; // increments to next available slot
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers.back().buffer); // gets the latest buffer
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderObj->getIndexCount() * sizeof(unsigned), renderObj->getIndices(), GL_STATIC_DRAW);
		}
		else {
			_buffers.push_back(Buffer_GL4(renderID, BUFF_Index_UI, _bufferSlots[_bufferIndex], 0)); // 0 indicates empty buffer
			_bufferIndex++; // increments to next available slot
		}

		// vertices generation
		_buffers.push_back(Buffer_GL4(renderID, BUFF_Vertex_Type, _bufferSlots[_bufferIndex], renderObj->getVertexCount()));
		_bufferIndex++; // increments to next available slot
		glBindBuffer(GL_ARRAY_BUFFER, _buffers.back().buffer); // gets the latest buffer
		glBufferData(GL_ARRAY_BUFFER, renderObj->getVertexCount() * sizeof(Geo_Vertex), renderObj->getVertices(), GL_STATIC_DRAW);

		// setting vertex input layout
		_vertexArrays.push_back(VertexArray_GL4(renderID, _vertexArraySlots[_vertexArrayIndex]));
		_vertexArrayIndex++; // increment to next available slot
		GL4::genVertexArrayLayout(&_vertexArrays.back(), _entryShader);
	}

	GLint blockCount;
	glGetProgramiv(_pipeline->shaderProg, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	if (blockCount == RENDER_BLOCK_SUPPORT) // Render uniforms supported
		glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
	else if (blockCount == SCENE_BLOCK_SUPPORT) { // Render and Scene uniforms supported
		glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
		glUniformBlockBinding(_pipeline->shaderProg, SCENE_BLOCK_INDEX, SCENE_BLOCK_BINDING);
	}

	_isBuilt = true;
}

#ifdef RASTERON_H

Img_Base Topl_Renderer_GL4::frame() {
	unsigned viewportHeight = Platform::getViewportHeight(_platformCtx.window);
	unsigned viewportWidth = Platform::getViewportWidth(_platformCtx.window);

	Rasteron_Image* stageImage = allocNewImg("frame", viewportWidth, viewportHeight);
	glReadPixels(0, 0, viewportHeight, viewportWidth, GL_RGBA, GL_UNSIGNED_BYTE, stageImage->data);
	Rasteron_Image* flipImage = createFlipImg(stageImage, FLIP_Upside); // flipping image over
	Rasteron_Image* mirrorImage = createMirrorImg(flipImage); // mirroring left and right sides
	switchRB(mirrorImage->data, viewportHeight * viewportWidth); // flipping red and blue bits

	_frameImage = Img_Base();
	_frameImage.setImage(mirrorImage);
	deleteImg(stageImage);
	deleteImg(flipImage);
	deleteImg(mirrorImage);
	return _frameImage;
}

void Topl_Renderer_GL4::attachTexture(const Rasteron_Image* rawImage, unsigned id) {
	GLuint textureTarget = _textureSlots[_textureIndex];
	_textureIndex++; // increments to next available slot

	for (std::vector<Texture_GL4>::iterator tex = _textures.begin(); tex != _textures.end(); tex++) {
		if (tex->renderID == id) {
			textureTarget = tex->texture;
			_textureIndex--; // decrement since texture binding is located
		}
	}

	glBindTexture(GL_TEXTURE_2D, textureTarget);

	GL4::setTextureProperties(GL_TEXTURE_2D, _texMode);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rawImage->height, rawImage->width, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawImage->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (_textureSlots[_textureIndex - 1] == textureTarget)
		_textures.push_back(Texture_GL4(id, TEX_2D, _texMode, textureTarget)); // Texture Addition
}

void Topl_Renderer_GL4::attachVolume(const Img_Volume* volume, unsigned id) {
	GLuint textureTarget = _textureSlots[_textureIndex];
	_textureIndex++; // increments to next available slot

	for (std::vector<Texture_GL4>::iterator tex = _textures.begin(); tex != _textures.end(); tex++) {
		if (tex->renderID == id) {
			textureTarget = tex->texture;
			_textureIndex--; // decrement since texture binding is located
		}
	}

	glBindTexture(GL_TEXTURE_3D, textureTarget);

	const Img_Base* volumeImage = volume->extractVolImage();
	GL4::setTextureProperties(GL_TEXTURE_3D, _texMode);
	glTexImage3D(
		GL_TEXTURE_3D,
		0, GL_RGBA,
		volume->getWidth(),
		volume->getHeight(),
		volume->getDepth(),
		0, GL_RGBA,
		GL_UNSIGNED_BYTE,
		volumeImage->getImage()->data
	);
	glGenerateMipmap(GL_TEXTURE_3D);

	if (_textureSlots[_textureIndex - 1] == textureTarget)
		_textures.push_back(Texture_GL4(id, TEX_3D, _texMode, textureTarget));
}

#endif

void Topl_Renderer_GL4::update(const Topl_Scene* scene) {
	blockBytes_t blockBytes;
	Buffer_GL4* targetBuff = nullptr;

	if (_buffers.front().renderID == SCENE_RENDER_ID) {
		_entryShader->genSceneBlock(scene, _activeCamera, &blockBytes);
		glBindBuffer(GL_UNIFORM_BUFFER, _buffers.front().buffer);
		unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
	}

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);

		_entryShader->genRenderBlock(actor, renderID, &blockBytes);
		for (std::vector<Buffer_GL4>::iterator buff = _buffers.begin(); buff < _buffers.end(); buff++)
			if (buff->renderID == renderID && buff->type == BUFF_Render_Block) targetBuff = &(*buff);

		if (targetBuff == nullptr) logMessage(MESSAGE_Exclaim, "Block buffer could not be located! ");
		else {
			glBindBuffer(GL_UNIFORM_BUFFER, targetBuff->buffer);
			unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
			glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
		}
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return;
}

void Topl_Renderer_GL4::drawMode() {
	switch (_drawMode) {
	case DRAW_Triangles: _drawMode_GL4 = GL_TRIANGLES; break;
	case DRAW_Points: _drawMode_GL4 = GL_POINTS; break;
	case DRAW_Lines: _drawMode_GL4 = GL_LINES; break;
	case DRAW_Fan: _drawMode_GL4 = GL_TRIANGLE_FAN; break;
	case DRAW_Strip: _drawMode_GL4 = GL_TRIANGLE_STRIP; break;
	default: return logMessage(MESSAGE_Exclaim, "Draw type not supported!");
	}
}

void Topl_Renderer_GL4::renderTarget(unsigned long renderID) {
	if (!_buffers.empty()) {
		if (renderID == SCENE_RENDER_ID && _buffers.front().renderID == SCENE_RENDER_ID) // Scene Target
			glBindBufferBase(GL_UNIFORM_BUFFER, SCENE_BLOCK_BINDING, _buffers.front().buffer);
		else { // Drawable Target
			for (std::vector<VertexArray_GL4>::iterator VAO = _vertexArrays.begin(); VAO < _vertexArrays.end(); VAO++)
				if (VAO->renderID == renderID) glBindVertexArray(VAO->vao);

			Buffer_GL4* vertexBuff = findBuffer(BUFF_Vertex_Type, renderID);
			Buffer_GL4* indexBuff = findBuffer(BUFF_Index_UI, renderID);
			Buffer_GL4* renderBlockBuff = findBuffer(BUFF_Render_Block, renderID);
			if (renderBlockBuff != nullptr)
				glBindBufferBase(GL_UNIFORM_BUFFER, RENDER_BLOCK_BINDING, renderBlockBuff->buffer);

			if (vertexBuff != nullptr && vertexBuff->count > 0) 
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuff->buffer);
			if (indexBuff != nullptr && indexBuff->count > 0) 
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff->buffer);

			for (unsigned t = 0; t < _textures.size(); t++)
				if (_textures.at(t).renderID == renderID) {
					glBindTexture(GL_TEXTURE_2D, _textures.at(t).texture);
					break;
				}

			// Draw Call!
			if (vertexBuff != nullptr && vertexBuff->count != 0) {
				if (indexBuff != nullptr && indexBuff->count != 0) glDrawElements(_drawMode_GL4, indexBuff->count, GL_UNSIGNED_INT, (void*)0);
				else glDrawArrays(_drawMode_GL4, 0, vertexBuff->count); // When no indices are present
			} 
			else logMessage(MESSAGE_Exclaim, "Corrupted Vertex Buffer!");

			// Unbinding
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}

Buffer_GL4* Topl_Renderer_GL4::findBuffer(BUFF_Type type, unsigned long renderID) {
	for (std::vector<Buffer_GL4>::iterator buffer = _buffers.begin(); buffer < _buffers.end(); buffer++)
		if (buffer->type == type && buffer->renderID == renderID)
			return &(*buffer);
	return nullptr; // error
}