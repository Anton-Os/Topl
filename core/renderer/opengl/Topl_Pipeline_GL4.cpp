#include "renderer/opengl/Topl_Renderer_GL4.hpp"

namespace GL4 {
	GLuint compileShader(std::string shaderText, GLenum shaderType) {
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

void Topl_Renderer_GL4::setPipeline(GL4::Pipeline* pipeline){
	if (pipeline == nullptr) return logMessage(MESSAGE_Exclaim, "Pipeline cannot be null!");

	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);
	
	if (_flags[PIPELINE_BIT]) glUseProgram(pipeline->shaderProg);
}

void Topl_Renderer_GL4::genPipeline(GL4::Pipeline* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, std::initializer_list<shader_cptr> shaders){
	if (pipeline == nullptr || ((vertexShader == nullptr || pixelShader == nullptr) && shaders.size() == 0))
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");

	// Vertex Shader
    if(vertexShader != nullptr){
        std::string vertexShaderSrc = vertexShader->getFileSource();
        pipeline->vertexShader = GL4::compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
        if (pipeline->vertexShader == 0){ pipeline->isReady = false; return; }
    }

	// Pixel Shader
    if(pixelShader != nullptr){
        std::string fragShaderSrc = pixelShader->getFileSource();
        pipeline->pixelShader = GL4::compileShader(fragShaderSrc, GL_FRAGMENT_SHADER);
        if (pipeline->pixelShader == 0){ pipeline->isReady = false; return; }
    }
#ifndef __ANDROID__
	auto geomShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_Geom; });
	auto tessCtrlShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_TessCtrl; });
	auto tessEvalShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_TessEval; });
	auto computeShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_Compute; });

	// Geometry Shader
	if (geomShader != shaders.end()) { // optional stage
		std::string geomShaderSrc = (*geomShader)->getFileSource();
		pipeline->geomShader = GL4::compileShader(geomShaderSrc, GL_GEOMETRY_SHADER);
		if(pipeline->geomShader == 0){ pipeline->isReady = false; return; }
	}

	// Tess. Control Shader
	if(tessCtrlShader != shaders.end()){ // optional stage
		std::string tessCtrlShaderSrc = (*tessCtrlShader)->getFileSource();
		pipeline->tessCtrlShader = GL4::compileShader(tessCtrlShaderSrc, GL_TESS_CONTROL_SHADER);
		if(pipeline->tessCtrlShader == 0){ pipeline->isReady = false; return; }
	}

	// Tess. Evaluation Shader
	if(tessEvalShader != shaders.end()){ // optional stage
		std::string tessEvalShaderSrc = (*tessEvalShader)->getFileSource();
		pipeline->tessEvalShader = GL4::compileShader(tessEvalShaderSrc, GL_TESS_EVALUATION_SHADER);
		if(pipeline->tessEvalShader == 0){ pipeline->isReady = false; return; }
	}

	// Compute Shader
	if(computeShader != shaders.end()){ // optional stage
		std::string computeShaderSrc = (*computeShader)->getFileSource();
		pipeline->computeShader = GL4::compileShader(computeShaderSrc, GL_COMPUTE_SHADER);
		if(pipeline->computeShader == 0){ pipeline->isReady = false; return; }
	}
#endif
	// Program Linking

	linkShaders(pipeline, vertexShader, pixelShader, shaders);
	if(pipeline->isReady == false) return logMessage(MESSAGE_Exclaim, "Error during pipeline generation");

	pipeline->setShaders(vertexShader, pixelShader, shaders);
	setPipeline(pipeline);

	GLint blockCount;
	glGetProgramiv(_pipeline->shaderProg, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	
	if(blockCount > 0) glUniformBlockBinding(_pipeline->shaderProg, RENDER_BLOCK_BINDING, RENDER_BLOCK_BINDING); // enables render block
	if(blockCount > 1) glUniformBlockBinding(_pipeline->shaderProg, SCENE_BLOCK_BINDING, SCENE_BLOCK_BINDING); // enables scene block
	if(blockCount > 2) glUniformBlockBinding(_pipeline->shaderProg, EXT_BLOCK_BINDING, EXT_BLOCK_BINDING); // enables extended block
}

void Topl_Renderer_GL4::linkShaders(GL4::Pipeline* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, std::initializer_list<shader_cptr> shaders){
	GLint result;

	auto geomShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_Geom; });
	auto tessCtrlShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_TessCtrl; });
	auto tessEvalShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_TessEval; });
	auto computeShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_Compute; });
	
	pipeline->shaderProg = glCreateProgram();
	glAttachShader(pipeline->shaderProg, pipeline->vertexShader);
	if(tessCtrlShader != shaders.end()) glAttachShader(pipeline->shaderProg, pipeline->tessCtrlShader);
	if(tessEvalShader != shaders.end()) glAttachShader(pipeline->shaderProg, pipeline->tessEvalShader);
	if(geomShader != shaders.end()) glAttachShader(pipeline->shaderProg, pipeline->geomShader);
	if(computeShader != shaders.end()) glAttachShader(pipeline->shaderProg, pipeline->computeShader);
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
		if(computeShader != nullptr) glDetachShader(pipeline->shaderProg, pipeline->computeShader);
		glDetachShader(pipeline->shaderProg, pipeline->pixelShader);
	}
}
