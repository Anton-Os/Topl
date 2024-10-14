#include "backends/opengl/Topl_Renderer_GL4.hpp"

namespace GL4 {
	static GLuint compileShader(std::string shaderText, GLenum shaderType) {
		GLint result;

		const char* source = shaderText.c_str();
		GLuint shader = glCreateShader(shaderType);
		if (!shaderText.empty()) glShaderSource(shader, 1, &source, NULL);
		else {
			perror("Shader File Empty!");
			return 0; // Error
		}

		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) { // On shader compilation error
			GLint maxLen;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLen);

			char* errorLog = (char*)malloc(maxLen * sizeof(char));
			glGetShaderInfoLog(shader, maxLen, &maxLen, errorLog);
			logMessage(MESSAGE_Exclaim, "Shader Compilation Error: \n" + std::string(source) + "\n");
			perror(errorLog);
			free(errorLog);

			return 0;
		}

		return shader;
	}
}

void Topl_Renderer_GL4::setPipeline(Topl_Pipeline_GL4* pipeline){
	if (pipeline == nullptr)
		return logMessage(MESSAGE_Exclaim, "Pipeline cannot be null!");

	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);
	
	if (_flags[PIPELINE_BIT])
		glUseProgram(pipeline->shaderProg);
}

void Topl_Renderer_GL4::genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	if (pipeline == nullptr || vertexShader == nullptr || pixelShader == nullptr)
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	else pipeline->isReady = true; // set to true until error encountered

	// Vertex Shader

	std::string vertexShaderSrc = vertexShader->getFileSource();
	pipeline->vertexShader = GL4::compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
	if (pipeline->vertexShader == 0) pipeline->isReady = false;

	// Pixel Shader

	std::string fragShaderSrc = pixelShader->getFileSource();
	pipeline->pixelShader = GL4::compileShader(fragShaderSrc, GL_FRAGMENT_SHADER);
	if (pipeline->pixelShader == 0) pipeline->isReady = false;

	// Program Linking

	linkShaders(pipeline, vertexShader, pixelShader, nullptr, nullptr, nullptr);
	if(pipeline->isReady == false) return logMessage(MESSAGE_Exclaim, "Error during pipeline generation");

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);

	GLint blockCount;
	glGetProgramiv(_pipeline->shaderProg, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);

	if(blockCount > 0) glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_BINDING, RENDER_BLOCK_BINDING); // enables render block
	if(blockCount > 1) glUniformBlockBinding(_pipeline->shaderProg, SCENE_BLOCK_BINDING, SCENE_BLOCK_BINDING); // enables scene block
	if(blockCount > 2) glUniformBlockBinding(_pipeline->shaderProg, EXT_BLOCK_BINDING, EXT_BLOCK_BINDING); // enables extended block
}

void Topl_Renderer_GL4::genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader){
	if (pipeline == nullptr || vertexShader == nullptr || pixelShader == nullptr)
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	else pipeline->isReady = true; // set to true until error encountered

	// Vertex Shader

	std::string vertexShaderSrc = vertexShader->getFileSource();
	pipeline->vertexShader = GL4::compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
	if (pipeline->vertexShader == 0) pipeline->isReady = false;

	// Pixel Shader

	std::string fragShaderSrc = pixelShader->getFileSource();
	pipeline->pixelShader = GL4::compileShader(fragShaderSrc, GL_FRAGMENT_SHADER);
	if (pipeline->pixelShader == 0) pipeline->isReady = false;

	// Geometry Shader

	if (geomShader != nullptr) { // optional stage
		std::string geomShaderSrc = geomShader->getFileSource();
		pipeline->geomShader = GL4::compileShader(geomShaderSrc, GL_GEOMETRY_SHADER);
		if(pipeline->geomShader == 0) pipeline->isReady = false;
	}

	// Tess. Control Shader

	if(tessCtrlShader != nullptr){ // optional stage
		std::string tessCtrlShaderSrc = tessCtrlShader->getFileSource();
		pipeline->tessCtrlShader = GL4::compileShader(tessCtrlShaderSrc, GL_TESS_CONTROL_SHADER);
		if(pipeline->tessCtrlShader == 0) pipeline->isReady = false;
	}

	// Tess. Evaluation Shader

	if(tessEvalShader != nullptr){ // optional stage
		std::string tessEvalShaderSrc = tessEvalShader->getFileSource();
		pipeline->tessEvalShader = GL4::compileShader(tessEvalShaderSrc, GL_TESS_EVALUATION_SHADER);
		if(pipeline->tessEvalShader == 0) pipeline->isReady = false;
	}

	// Program Linking

	linkShaders(pipeline, vertexShader, pixelShader, geomShader, tessCtrlShader, tessEvalShader);
	if(pipeline->isReady == false) return logMessage(MESSAGE_Exclaim, "Error during pipeline generation");


	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);

	GLint blockCount;
	glGetProgramiv(_pipeline->shaderProg, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	
	if(blockCount > 0) glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_BINDING, RENDER_BLOCK_BINDING); // enables render block
	if(blockCount > 1) glUniformBlockBinding(_pipeline->shaderProg, SCENE_BLOCK_BINDING, SCENE_BLOCK_BINDING); // enables scene block
	if(blockCount > 2) glUniformBlockBinding(_pipeline->shaderProg, EXT_BLOCK_BINDING, EXT_BLOCK_BINDING); // enables extended block
}

void Topl_Renderer_GL4::linkShaders(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader){
	GLint result;
	
	pipeline->shaderProg = glCreateProgram();
	glAttachShader(pipeline->shaderProg, pipeline->vertexShader);
	if(tessCtrlShader != nullptr) glAttachShader(pipeline->shaderProg, pipeline->tessCtrlShader);
	if(tessEvalShader != nullptr) glAttachShader(pipeline->shaderProg, pipeline->tessEvalShader);
	if(geomShader != nullptr) glAttachShader(pipeline->shaderProg, pipeline->geomShader);
	glAttachShader(pipeline->shaderProg, pipeline->pixelShader);
	glLinkProgram(pipeline->shaderProg);

	glGetProgramiv(pipeline->shaderProg, GL_LINK_STATUS, &result);
	if (result == GL_FALSE){
		GLint maxLen;
		glGetProgramiv(pipeline->shaderProg, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetProgramInfoLog(pipeline->shaderProg, maxLen, &maxLen, errorLog);
		logMessage(MESSAGE_Exclaim, "Shader Linking Error!\n");
		perror(errorLog);
		free(errorLog);

		pipeline->isReady = false;
		return;
	}
	else { // detach after successful link
		glDetachShader(pipeline->shaderProg, pipeline->vertexShader);
		if(tessCtrlShader != nullptr) glDetachShader(pipeline->shaderProg, pipeline->tessCtrlShader);
		if(tessEvalShader != nullptr) glDetachShader(pipeline->shaderProg, pipeline->tessEvalShader);
		if(geomShader != nullptr) glDetachShader(pipeline->shaderProg, pipeline->geomShader);
		glDetachShader(pipeline->shaderProg, pipeline->pixelShader);
	}
}