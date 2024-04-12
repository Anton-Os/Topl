#include "backends/opengl/Topl_Renderer_GL4.hpp"

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
	glDeleteBuffers(MAX_RENDERID, _bufferSlots);
	glDeleteVertexArrays(MAX_RENDERID, _vertexArraySlots);
	glDeleteTextures(MAX_RENDERID, _textureSlots);

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
#elif defined(__linux__)
static void init_linux(GLXContext graphicsContext, Display* display, Window* window) {
	GLint visualInfoAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo* visualInfo = glXChooseVisual(display, 0, visualInfoAttribs);

	graphicsContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
	glXMakeCurrent(display, *window, graphicsContext);
}
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

	_bufferSlots = (GLuint*)malloc(MAX_RENDERID* sizeof(GLuint));
	_vertexArraySlots = (GLuint*)malloc(MAX_RENDERID * sizeof(GLuint));
	_textureSlots = (GLuint*)malloc(MAX_RENDERID * sizeof(GLuint));

	glGenBuffers(MAX_RENDERID, _bufferSlots);
	glGenVertexArrays(MAX_RENDERID, _vertexArraySlots);
	glGenTextures(MAX_RENDERID, _textureSlots);

	// glDisable(GL_CULL_FACE);

	glLineWidth(1.5f);
	glPointSize(3.0f);
}

void Topl_Renderer_GL4::clear() {
	glClearColor(CLEAR_R, CLEAR_G, CLEAR_B, CLEAR_A);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Topl_Renderer_GL4::setViewport(const Topl_Viewport* viewport) {
	unsigned x = viewport->xOffset;
	unsigned y = -viewport->yOffset - viewport->height + Platform::getViewportHeight(_platformCtx.window);

	if (viewport != nullptr)
		glViewport(x, y, viewport->width, viewport->height);
	else _flags[BUILD_BIT] = false; // Error
}

#ifdef _WIN32
static inline void swapBuffers_win(HDC* windowDC) { SwapBuffers(*windowDC); }
#elif defined(__linux__)
static void swapBuffers_linux(Display* display, Window* window) { glXSwapBuffers(display, *window); }
#endif

void Topl_Renderer_GL4::swapBuffers(double frameTime) {
#ifdef _WIN32
		swapBuffers_win(&_platformCtx.deviceCtx);
#elif defined(__linux__)
		swapBuffers_linux(_platformCtx.display, &_platformCtx.window);
#endif
}

void Topl_Renderer_GL4::build(const Topl_Scene* scene) {
	blockBytes_t shaderBlockData;

	// scene block buffer generation
	shaderBlockData.clear();
	_entryShader->genSceneBlock(scene, _activeCamera, &shaderBlockData);
	_blockBufferMap.insert({ SCENE_RENDERID, Buffer_GL4(*(_bufferSlots + _bufferIndex)) });
	_bufferIndex++; // increments to next available slot
	glBindBuffer(GL_UNIFORM_BUFFER, _blockBufferMap.at(SCENE_RENDERID).buffer);
	unsigned blockSize = sizeof(uint8_t) * shaderBlockData.size();
	glBufferData(GL_UNIFORM_BUFFER, blockSize, shaderBlockData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for (unsigned g = 0; g < scene->getActorCount(); g++) { // TODO: Detect and rebuild with deleted or added objects
		actor_cptr actor = scene->getGeoActor(g);
		Geo_Mesh* mesh = (Geo_Mesh*)actor->getMesh();
		unsigned long renderID = getRenderID(actor);
		
		unsigned vertexBuffIndex = 0, indexBuffIndex = 0, renderBlockBuffIndex = 0;
		if(renderID == INVALID_RENDERID){ // Actor will not be duplicated
			_renderIDs++;
			_renderObjMap.insert({ _renderIDs, scene->getGeoActor(g) });
			_renderTargetMap.insert({ scene->getGeoActor(g), _renderIDs });
			renderID = getRenderID(actor);
		} else { // old data must be replaced
			/* auto vertexBuff = std::find_if(_buffers.begin(), _buffers.end(), [renderID](const Buffer_GL4& b) { return b.type == BUFF_Vertex_Type && b.renderID == renderID; });
			vertexBuffIndex = (*vertexBuff).buffer;
			if(vertexBuff != _buffers.end()) _buffers.erase(vertexBuff);
			auto indexBuff = std::find_if(_buffers.begin(), _buffers.end(), [renderID](const Buffer_GL4& b) { return b.type == BUFF_Index_UI && b.renderID == renderID; });
			indexBuffIndex = (*indexBuff).buffer;
			if(indexBuff != _buffers.end()) _buffers.erase(indexBuff);
			auto renderBlockBuff = std::find_if(_buffers.begin(), _buffers.end(), [renderID](const Buffer_GL4& b) { return b.type == BUFF_Render_Block && b.renderID == renderID; });
			renderBlockBuffIndex = (*renderBlockBuff).buffer;
			if(renderBlockBuff != _buffers.end()) _buffers.erase(renderBlockBuff); */
		}

		// render block buffer generation
		shaderBlockData.clear();
		_entryShader->genRenderBlock(actor, &shaderBlockData);
		_blockBufferMap.insert({ renderID, Buffer_GL4(renderID, BUFF_Render_Block, *(_bufferSlots + ((renderBlockBuffIndex != 0)? renderBlockBuffIndex : _bufferIndex))) });
		if(renderBlockBuffIndex == 0) _bufferIndex++; // increments to next available slot
		glBindBuffer(GL_UNIFORM_BUFFER, _blockBufferMap.at(renderID).buffer);
		unsigned blockSize = sizeof(uint8_t) * shaderBlockData.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, shaderBlockData.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// indices generation
		(mesh->getIndices() != nullptr)
			? _indexBufferMap.insert({ renderID, Buffer_GL4(renderID, BUFF_Index_UI, *(_bufferSlots + ((indexBuffIndex != 0)? indexBuffIndex : _bufferIndex)), mesh->getIndexCount()) })
			: _indexBufferMap.insert({ renderID, Buffer_GL4(renderID, BUFF_Index_UI, *(_bufferSlots + ((indexBuffIndex != 0)? indexBuffIndex : _bufferIndex)), 0) });
		if (mesh->getIndices() != nullptr) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferMap.at(renderID).buffer); // gets the latest buffer
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndexCount() * sizeof(unsigned), mesh->getIndices(), GL_STATIC_DRAW);
		}
		if(indexBuffIndex == 0) _bufferIndex++; // increments to next available slot

		// vertices generation
		_vertexBufferMap.insert({ renderID, Buffer_GL4(renderID, BUFF_Vertex_Type, *(_bufferSlots + ((vertexBuffIndex != 0)? vertexBuffIndex : _bufferIndex)), mesh->getVertexCount()) });
		if(vertexBuffIndex == 0) _bufferIndex++; // increments to next available slot
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferMap.at(renderID).buffer); // gets the latest buffer
		glBufferData(GL_ARRAY_BUFFER, mesh->getVertexCount() * sizeof(Geo_Vertex), mesh->getVertices(), GL_STATIC_DRAW);

		// setting vertex input layout
		_vertexArrayMap.insert({ renderID, VertexArray_GL4(renderID, *(_vertexArraySlots + _vertexArrayIndex)) });
		_vertexArrayIndex++; // increment to next available slot
		GL4::genVertexArrayLayout(&_vertexArrayMap.at(renderID), _entryShader);
	}

	GLint blockCount;
	glGetProgramiv(_pipeline->shaderProg, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	if (blockCount == RENDER_BLOCK_SUPPORT) // Render uniforms supported
		glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
	else if (blockCount == SCENE_BLOCK_SUPPORT) { // Render and Scene uniforms supported
		glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_INDEX, RENDER_BLOCK_BINDING);
		glUniformBlockBinding(_pipeline->shaderProg, SCENE_BLOCK_INDEX, SCENE_BLOCK_BINDING);
	}

	_flags[BUILD_BIT] = true;
}

#ifdef RASTERON_H

Img_Base Topl_Renderer_GL4::frame() {
	unsigned viewportHeight = Platform::getViewportHeight(_platformCtx.window);
	unsigned viewportWidth = Platform::getViewportWidth(_platformCtx.window);

	Rasteron_Image* stageImage = RASTERON_ALLOC("frame", viewportWidth, viewportHeight);
	glReadPixels(0, 0, viewportHeight, viewportWidth, GL_RGBA, GL_UNSIGNED_BYTE, stageImage->data);

	Rasteron_Image* flipImage = flipImgOp(stageImage, FLIP_Upside); // flipping image over
	Rasteron_Image* mirrorImage = mirrorImgOp(flipImage); // mirroring left and right sides
	bitSwitch_RB(mirrorImage->data, viewportHeight * viewportWidth); // flipping red and blue bits

	_frameImage = Img_Base();
	_frameImage.setImage(mirrorImage);
	RASTERON_DEALLOC(stageImage); RASTERON_DEALLOC(flipImage); RASTERON_DEALLOC(mirrorImage);
	return _frameImage;
}

void Topl_Renderer_GL4::attachTexAt(const Rasteron_Image* image, unsigned renderID, unsigned binding) {
	GLuint textureTarget = *(_textureSlots + _textureIndex);
	_textureIndex++; // increments to next available slot

	for (std::vector<Texture_GL4>::iterator tex = _textures.begin(); tex != _textures.end(); tex++) {
		if (tex->renderID == renderID && tex->binding == binding && tex->format == TEX_2D) { // multi-texture substitution
			textureTarget = tex->texture;
			if (_textureIndex > 1) _textureIndex--; // decrement if texture binding is located
		}
	}

	glActiveTexture(GL_TEXTURE0 + binding);
	glBindTexture(GL_TEXTURE_2D, textureTarget);

	GL4::setTextureProperties(GL_TEXTURE_2D, _texMode);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->height, image->width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (*(_textureSlots + _textureIndex - 1) == textureTarget)
		_textures.push_back(Texture_GL4(renderID, (unsigned short)binding, TEX_2D, _texMode, textureTarget)); // multi-texture addition
}

void Topl_Renderer_GL4::attachTex3D(const Img_Volume* volumeTex, unsigned renderID) {
	GLuint textureTarget = *(_textureSlots + _textureIndex);
	_textureIndex++; // increments to next available slot

	for (std::vector<Texture_GL4>::iterator tex = _textures.begin(); tex != _textures.end(); tex++) {
		if (tex->renderID == renderID && tex->format == TEX_3D) {
			textureTarget = tex->texture;
			if (_textureIndex > 1) _textureIndex--; // decrement since texture binding is located
		}
	}

	glActiveTexture(GL_TEXTURE0 + MAX_TEX_BINDINGS);
	glBindTexture(GL_TEXTURE_3D, textureTarget);

	const Img_Base* volumeTexImage = volumeTex->extractVolImage();
	GL4::setTextureProperties(GL_TEXTURE_3D, _texMode);
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
		_textures.push_back(Texture_GL4(renderID, TEX_3D, _texMode, textureTarget));
}

#endif

void Topl_Renderer_GL4::update(const Topl_Scene* scene) {
	blockBytes_t shaderBlockData;

	if (scene != ALL_SCENES && _blockBufferMap.at(SCENE_RENDERID).renderID == SCENE_RENDERID) {
		shaderBlockData.clear();
		_entryShader->genSceneBlock(scene, _activeCamera, &shaderBlockData);
		glBindBuffer(GL_UNIFORM_BUFFER, _blockBufferMap.at(SCENE_RENDERID).buffer);
		unsigned blockSize = sizeof(uint8_t) * shaderBlockData.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, shaderBlockData.data(), GL_STATIC_DRAW);
	}

	for (unsigned g = (scene != ALL_SCENES)? 0 : 1; g < ((scene != ALL_SCENES)? scene->getActorCount() : _renderIDs); g++) {
		actor_cptr actor = (scene != ALL_SCENES)? scene->getGeoActor(g) : _renderObjMap[g];
		unsigned renderID = (scene != ALL_SCENES)? getRenderID(actor) : g;
		if (_blockBufferMap.find(renderID) != _blockBufferMap.end()){ // Shader Render Block
			shaderBlockData.clear();
			_entryShader->genRenderBlock(actor, &shaderBlockData);
			glBindBuffer(GL_UNIFORM_BUFFER, _blockBufferMap.at(renderID).buffer);
			unsigned blockSize = sizeof(uint8_t) * shaderBlockData.size();
			glBufferData(GL_UNIFORM_BUFFER, blockSize, shaderBlockData.data(), GL_STATIC_DRAW);
		}
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
	default: return logMessage(MESSAGE_Exclaim, "Draw type not supported!");
	}
}

void Topl_Renderer_GL4::draw(const Geo_Actor* actor) {
	unsigned long renderID = _renderTargetMap[actor];
	// static Buffer_GL4 *sceneBlockBuff, *renderBlockBuff, *vertexBuff, *indexBuff;
	if (renderID == SCENE_RENDERID && _blockBufferMap.at(SCENE_RENDERID).renderID == SCENE_RENDERID) // Scene Target
		glBindBufferBase(GL_UNIFORM_BUFFER, SCENE_BLOCK_BINDING, _blockBufferMap.at(SCENE_RENDERID).buffer);
	else if (renderID != SCENE_RENDERID && actor->isShown) { // Drawable Target
		// Data & Buffer Updates
		
		if(_vertexArrayMap.find(renderID) != _vertexArrayMap.end()) glBindVertexArray(_vertexArrayMap.at(renderID).vao);

		if(_vertexBufferMap.find(renderID) != _vertexBufferMap.end()) glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferMap.at(renderID).buffer);
		if(_indexBufferMap.find(renderID) != _indexBufferMap.end()) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferMap.at(renderID).buffer);
		if(_blockBufferMap.find(renderID) != _blockBufferMap.end()) glBindBufferBase(GL_UNIFORM_BUFFER, RENDER_BLOCK_BINDING, _blockBufferMap.at(renderID).buffer);

		// Texture Updates 
	
		auto tex2D = std::find_if(_textures.begin(), _textures.end(), [renderID](const Texture_GL4& t){ return t.renderID == renderID && t.format == TEX_2D && t.binding == 0; });
		if (tex2D != _textures.end()){
			glActiveTexture(GL_TEXTURE0 + tex2D->binding);
			glBindTexture(GL_TEXTURE_2D, tex2D->texture);
		}
		/* for(unsigned b = 1; b < MAX_TEX_BINDINGS; b++){
			tex2D = std::find_if(_textures.begin(), _textures.end(), [renderID, b](const Texture_GL4& t){ return t.renderID == renderID && t.format == TEX_2D && t.binding == b; });
			if (tex2D != _textures.end()){
				glActiveTexture(GL_TEXTURE0 + tex2D->binding);
				glBindTexture(GL_TEXTURE_2D, tex2D->texture);
			}
		} */
		auto tex3D = std::find_if(_textures.begin(), _textures.end(), [renderID](const Texture_GL4& t){ return t.renderID == renderID && t.format == TEX_3D; });
		if(tex3D != _textures.end()){
			glActiveTexture(GL_TEXTURE0 + MAX_TEX_BINDINGS);
			glBindTexture(GL_TEXTURE_3D, tex3D->texture);
		}

		// Draw Call!
		if (_vertexBufferMap.find(renderID) != _vertexBufferMap.end()) {
			if (_indexBufferMap.find(renderID) != _indexBufferMap.end()) 
				glDrawElements(_drawMode_GL4, _indexBufferMap.at(renderID).count, GL_UNSIGNED_INT, (void*)0); // indexed draw
			else glDrawArrays(_drawMode_GL4, 0, _vertexBufferMap.at(renderID).count); // non-indexed draw
		} 
		// TODO: Include instanced draw call
		else logMessage(MESSAGE_Exclaim, "Corrupt Vertex Buffer!");

		// Unbinding
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}