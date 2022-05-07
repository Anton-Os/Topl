#include "opengl/Topl_Renderer_GL4.hpp"

namespace Renderer {
	// Shader Functions

	static GLenum getShaderFormat(enum SHDR_ValueType type){
		switch(type) {
		case SHDR_float_vec4: case SHDR_float_vec3: case SHDR_float_vec2: case SHDR_float: 
			return GL_FLOAT;
		case SHDR_uint_vec4: case SHDR_uint_vec3: case SHDR_uint_vec2: case SHDR_uint: 
			return GL_UNSIGNED_INT;
		default:
		 	logMessage(MESSAGE_Exclaim, "GL4 Shader Input Type Not Supported!");
			return 0; // error
		}
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
	cleanup_win(&_platformCtx.window, &_platformCtx.deviceCtx, &_platformCtx.oglCtx);
#elif defined(__linux__)
	cleanup_linux(_platformCtx.display, _platformCtx.oglCtx);
#endif
}


void Topl_Renderer_GL4::init(NATIVE_WINDOW window){
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

	if (_viewports == nullptr) {
		_isBuilt = false;
		return; // Error // Viewports Dont Exist
	}
	if(_viewportCount <= 1) // singular viewport
		glViewport(_viewports->xOffset, _viewports->yOffset, _viewports->width, _viewports->height);
	else // multiple viewports
		for (unsigned short v = 0; v < _viewportCount; v++) {
			Topl_Viewport* viewport = _viewports + v;
			glViewportIndexedf(v, viewport->xOffset, viewport->yOffset, viewport->width, viewport->height);
		}
}

void Topl_Renderer_GL4::clearView(){
	glClearColor(CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_ALPHA);
	// glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Topl_Renderer_GL4::switchFramebuff(){
	if(_isDrawn)
#ifdef _WIN32 // Swap buffers in windows
	swapBuffers_win(&_platformCtx.deviceCtx);
#elif defined(__linux__)
	swapBuffers_linux(_platformCtx.display, &_platformCtx.window);
#endif
	_isDrawn = false; // awaiting another draw call
}

void Topl_Renderer_GL4::build(const Topl_Scene* scene){
	blockBytes_t blockBytes; // container for constant and uniform buffer updates
	// scene uniform block buffer generation
	if (_entryShader->genSceneBlock(scene, _activeCamera, &blockBytes)) {
		_buffers.push_back(Buffer_GL4(_bufferSlots[_bufferIndex])); 
		_bufferIndex++; // increments to next available slot
		glBindBuffer(GL_UNIFORM_BUFFER, _buffers.back().buffer);
		unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	 for (unsigned g = 0; g < scene->getActorCount(); g++) {
		_renderIDs++;
		_renderTargets_map.insert({ _renderIDs, scene->getGeoActor(g) });
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);
		Geo_RenderObj* actor_renderObj = (Geo_RenderObj*)actor->getRenderObj();

		vertex_cptr_t actor_vData = actor_renderObj->getVertices();
		ui_cptr_t actor_iData = actor_renderObj->getIndices();

		// render block buffer generation
		if (_entryShader->genGeoBlock(actor, &blockBytes)) {
			_buffers.push_back(Buffer_GL4(renderID, BUFF_Render_Block, _bufferSlots[_bufferIndex]));
			_bufferIndex++; // increments to next available slot
			glBindBuffer(GL_UNIFORM_BUFFER, _buffers.back().buffer);
			unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
			glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		// index creation
		if (actor_iData != nullptr) {
			_buffers.push_back(Buffer_GL4(renderID, BUFF_Index_UI, _bufferSlots[_bufferIndex], actor_renderObj->getIndexCount()));
			_bufferIndex++; // increments to next available slot
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers.back().buffer); // Gets the latest buffer for now
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, actor_renderObj->getIndexCount() * sizeof(unsigned), actor_iData, GL_STATIC_DRAW);
		} else {
			_buffers.push_back(Buffer_GL4(renderID, BUFF_Index_UI, _bufferSlots[_bufferIndex], 0)); // 0 indicates empty buffer
			_bufferIndex++; // increments to next available slot
		}

		_buffers.push_back(Buffer_GL4(renderID, BUFF_Vertex_Type, _bufferSlots[_bufferIndex], actor_renderObj->getVertexCount()));
		_bufferIndex++; // increments to next available slot
		glBindBuffer(GL_ARRAY_BUFFER, _buffers.back().buffer); // Gets the latest buffer for now
		glBufferData(GL_ARRAY_BUFFER, actor_renderObj->getVertexCount() * sizeof(Geo_Vertex), actor_vData, GL_STATIC_DRAW);

		_vertexArrays.push_back(VertexArray_GL4(renderID, _vertexArraySlots[_vertexArrayIndex]));
		_vertexArrayIndex++; // increment to next available slot
		VertexArray_GL4* VAO_ptr = &_vertexArrays.back(); // Check to see if all parameters are valid
		glBindVertexArray(VAO_ptr->vao);

		GLsizei inputElementOffset = 0;
		for(unsigned i = 0; i < _entryShader->getInputCount(); i++){
			const Shader_Type* shaderType = _entryShader->getInputAtIndex(i);

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,
				(GLint)Topl_Pipeline::getSize(shaderType->type),
				Renderer::getShaderFormat(shaderType->type),
				GL_FALSE,
				sizeof(Geo_Vertex),
				(inputElementOffset != 0) ? GL4_BUFFER_OFFSET(inputElementOffset) : NULL
			);

			inputElementOffset += Topl_Pipeline::getOffset(shaderType->type);
		}
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

Rasteron_Image* Topl_Renderer_GL4::frame(){
	// TODO: Crop to viewport!
	// see https://stackoverflow.com/questions/29764925/how-to-get-set-the-width-and-height-of-the-default-framebuffer
	unsigned viewportHeight = Platform::getViewportHeight(_platformCtx.window);
	unsigned viewportWidth = Platform::getViewportWidth(_platformCtx.window);

	Rasteron_Image* rawImage = allocNewImg("framebuff", TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH);
	glReadPixels(0, 0, rawImage->width, rawImage->height, GL_RGBA, GL_UNSIGNED_BYTE, rawImage->data);
	Rasteron_Image* flipImage = createImgFlip(rawImage, FLIP_Upside);
	// Rasteron_Image* image = allocNewImg("framebuff-crop", viewportHeight, viewportWidth);

	deleteImg(rawImage);
	// deleteImg(flipImage);
	return flipImage;
}

void Topl_Renderer_GL4::texturize(const Topl_Scene* scene) {
#ifdef RASTERON_H // Rasteron dependency required for updating textures
	// Need to clear saved textures entirely for texture update
	_textures.clear();
	glDeleteTextures(GL4_TEXTURE_BINDINGS_MAX, &_textureSlots[0]);
	_textureIndex = 0;

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);

		const Rasteron_Image* texture = scene->getTexture(actor->getName());
		if (texture != nullptr) attachTexture(texture, renderID);

		// const Topl_Material* material = scene->getMaterial(actor->getName());
		// if (material != nullptr) attachMaterial(texture, renderID);
	}
#endif
}

void Topl_Renderer_GL4::attachTexture(const Rasteron_Image* rawImage, unsigned id){
	GLuint texture = _textureSlots[_textureIndex];
	_textureIndex++; // increments to next available slot

	glBindTexture(GL_TEXTURE_2D, texture);

	Renderer::setTextureProperties(GL_TEXTURE_2D, _texMode); // setting texture mode properties
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rawImage->height, rawImage->width, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawImage->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	_textures.push_back(Texture_GL4(id, TEX_2D, _texMode, texture));
}

void Topl_Renderer_GL4::attachMaterial(const Topl_Material* material, unsigned id) {
	GLuint texture = _textureSlots[_textureIndex];
	_textureIndex++; // increments to next available slot

	glBindTexture(GL_TEXTURE_3D, texture);

	Renderer::setTextureProperties(GL_TEXTURE_3D, _texMode); // setting texture mode properties
	// glTexImage3D(); // Generate Data Here!!!
	glGenerateMipmap(GL_TEXTURE_3D);

	_textures.push_back(Texture_GL4(id, TEX_3D, _texMode, texture));
}

#endif

void Topl_Renderer_GL4::update(const Topl_Scene* scene){
	blockBytes_t blockBytes;
	Buffer_GL4* targetBuff = nullptr;

	if (_entryShader->genSceneBlock(scene, _activeCamera, &blockBytes) && _buffers.front().renderID == SPECIAL_SCENE_RENDER_ID) {
		glBindBuffer(GL_UNIFORM_BUFFER, _buffers.front().buffer);
		unsigned blockSize = sizeof(uint8_t) * blockBytes.size();
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBytes.data(), GL_STATIC_DRAW);
	}

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);

		if (_entryShader->genGeoBlock(actor, &blockBytes)) {
			for (std::vector<Buffer_GL4>::iterator buff = _buffers.begin(); buff < _buffers.end(); buff++)
				if (buff->renderID == renderID && buff->type == BUFF_Render_Block) targetBuff = &(*buff);

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

/* void Topl_Renderer_GL4::render(const Topl_Scene* scene){
	if (_buffers.front().renderID == SPECIAL_SCENE_RENDER_ID) // passing scene block buffer to shader
		glBindBufferBase(GL_UNIFORM_BUFFER, SCENE_BLOCK_BINDING, _buffers.front().buffer);

	// Rendering Loop!
	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);
		if (renderID == 0) {
			_isDrawn = false;
			return logMessage(MESSAGE_Exclaim, "renderID not found!");
		}

		for (std::vector<VertexArray_GL4>::iterator VAO = _vertexArrays.begin(); VAO < _vertexArrays.end(); VAO++)
			if (VAO->renderID == renderID) glBindVertexArray(VAO->vao);
			else continue; // if it continues all the way through error has occured

		Buffer_GL4* vertexBuff = findBuffer(BUFF_Vertex_Type, renderID);
		Buffer_GL4* indexBuff = findBuffer(BUFF_Index_UI, renderID);
		Buffer_GL4* renderBlockBuff = findBuffer(BUFF_Render_Block, renderID);
		if (renderBlockBuff != nullptr)
			glBindBufferBase(GL_UNIFORM_BUFFER, RENDER_BLOCK_BINDING, renderBlockBuff->buffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuff->buffer);
		if(indexBuff != nullptr) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff->buffer);

		for (unsigned t = 0; t < _textures.size(); t++) {
			if (_textures.at(t).renderID > renderID) break; // Geometry actor is passed in sequence 
			else if (_textures.at(t).renderID == renderID) {
				glBindTexture(GL_TEXTURE_2D, _textures.at(t).texture);
				break;
			}
		}

		// Draw Call!
		if (indexBuff != nullptr && indexBuff->count != 0) glDrawElements(_drawMode_GL4, indexBuff->count, GL_UNSIGNED_INT, (void*)0);
		else glDrawArrays(_drawMode_GL4, 0, vertexBuff->count); // When no indices are present

		// Unbinding
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	_isDrawn = true;
} */

void Topl_Renderer_GL4::renderTarget(unsigned long renderID) {
	if (renderID == SPECIAL_SCENE_RENDER_ID && _buffers.front().renderID == SPECIAL_SCENE_RENDER_ID)
		glBindBufferBase(GL_UNIFORM_BUFFER, SCENE_BLOCK_BINDING, _buffers.front().buffer);
	else {
		for (std::vector<VertexArray_GL4>::iterator VAO = _vertexArrays.begin(); VAO < _vertexArrays.end(); VAO++)
			if (VAO->renderID == renderID) glBindVertexArray(VAO->vao);

		Buffer_GL4* vertexBuff = findBuffer(BUFF_Vertex_Type, renderID);
		Buffer_GL4* indexBuff = findBuffer(BUFF_Index_UI, renderID);
		Buffer_GL4* renderBlockBuff = findBuffer(BUFF_Render_Block, renderID);
		if (renderBlockBuff != nullptr)
			glBindBufferBase(GL_UNIFORM_BUFFER, RENDER_BLOCK_BINDING, renderBlockBuff->buffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuff->buffer);
		if(indexBuff != nullptr) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff->buffer);

		for (unsigned t = 0; t < _textures.size(); t++) {
			if (_textures.at(t).renderID > renderID) break; // geometry actor is passed in sequence 
			else if (_textures.at(t).renderID == renderID) {
				glBindTexture(GL_TEXTURE_2D, _textures.at(t).texture);
				break;
			}
		}

		// Draw Call!
		if (indexBuff != nullptr && indexBuff->count != 0) glDrawElements(_drawMode_GL4, indexBuff->count, GL_UNSIGNED_INT, (void*)0);
		else glDrawArrays(_drawMode_GL4, 0, vertexBuff->count); // When no indices are present

		// Unbinding
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

Buffer_GL4* Topl_Renderer_GL4::findBuffer(BUFF_Type type, unsigned long renderID){
	for (std::vector<Buffer_GL4>::iterator buffer = _buffers.begin(); buffer < _buffers.end(); buffer++)
		if (buffer->type == type && buffer->renderID == renderID)
			return &(*buffer);
	return nullptr; // error
}