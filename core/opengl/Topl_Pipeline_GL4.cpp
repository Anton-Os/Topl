#include "opengl/Topl_Renderer_GL4.hpp"

namespace Renderer {
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
		if (result == GL_FALSE) return 0;

		return shader;
	}

	// Error Handling

	void errorShaderCompile(const char* shaderName, GLuint shader) {
		perror("Shader compilation failed"); // Add more robust error checking

		GLint maxLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetShaderInfoLog(shader, maxLen, &maxLen, errorLog);
		perror(errorLog);
		free(errorLog);

		return;
	}

	void errorProgramLink(GLuint shaderProg){
		perror("Shader program failed to link");

		GLint maxLen;
		glGetProgramiv(shaderProg, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetProgramInfoLog(shaderProg, maxLen, &maxLen, errorLog);
		perror(errorLog);
		free(errorLog);

		return;
	}
}

void Topl_Renderer_GL4::setPipeline(Topl_Pipeline_GL4* pipeline){
	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);
	
	if (_isPipelineReady)
		glUseProgram(pipeline->shaderProg);
}

void Topl_Renderer_GL4::genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	// Vertex Shader

	std::string vertexShaderSrc = readFile(vertexShader->getFilePath());
	pipeline->vShader = Renderer::compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
	if (pipeline->vShader == 0) {
		Renderer::errorShaderCompile("Vertex", pipeline->vShader);
		pipeline->isReady = false;
	}

	// Fragment Shader

	std::string fragShaderSrc = readFile(pixelShader->getFilePath());
	pipeline->fShader = Renderer::compileShader(fragShaderSrc, GL_FRAGMENT_SHADER);
	if (pipeline->fShader == 0) {
		Renderer::errorShaderCompile("Fragment", pipeline->fShader);
		pipeline->isReady = false;
	}

	// Shader Program Creation and Linking
	GLint result;

	pipeline->shaderProg = glCreateProgram();
	glAttachShader(pipeline->shaderProg, pipeline->vShader);
	glAttachShader(pipeline->shaderProg, pipeline->fShader);
	glLinkProgram(pipeline->shaderProg);

	glGetProgramiv(pipeline->shaderProg, GL_LINK_STATUS, &result);
	if (result == GL_FALSE){
		Renderer::errorProgramLink(pipeline->shaderProg);
		pipeline->isReady = false;
	}
	else { // detach after successful link
		glDetachShader(pipeline->shaderProg, pipeline->vShader);
		glDetachShader(pipeline->shaderProg, pipeline->fShader);
	}

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}

void Topl_Renderer_GL4::genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
	// Vertex Shader

	std::string vertexShaderSrc = readFile(vertexShader->getFilePath());
	pipeline->vShader = Renderer::compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
	if (pipeline->vShader == 0) {
		Renderer::errorShaderCompile("Vertex", pipeline->vShader);
		pipeline->isReady = false;
	}

	// Tess. Control Shader

	if(tessCtrlShader != nullptr){ // optional stage
		std::string tessCtrlShaderSrc = readFile(tessCtrlShader->getFilePath());
		pipeline->tcShader = Renderer::compileShader(tessCtrlShaderSrc, GL_TESS_CONTROL_SHADER);
		if (pipeline->tcShader == 0) {
			Renderer::errorShaderCompile("Tess. Control", pipeline->tcShader);
			pipeline->isReady = false;
		}
	}

	// Tess. Evaluation Shader

	if(tessEvalShader != nullptr){ // optional stage
		std::string tessEvalShaderSrc = readFile(tessEvalShader->getFilePath());
		pipeline->teShader = Renderer::compileShader(tessEvalShaderSrc, GL_TESS_EVALUATION_SHADER);
		if (pipeline->teShader == 0) {
			Renderer::errorShaderCompile("Tess. Eval", pipeline->teShader);
			pipeline->isReady = false;
		}
	}

	// Geometry Shader

	if(geomShader != nullptr){ // optional stage
		std::string geomShaderSrc = readFile(geomShader->getFilePath());
		pipeline->gShader = Renderer::compileShader(geomShaderSrc, GL_GEOMETRY_SHADER);
		if (pipeline->gShader == 0) {
			Renderer::errorShaderCompile("Geometry", pipeline->gShader);
			pipeline->isReady = false;
		}
	}


	// Fragment Shader

	std::string fragShaderSrc = readFile(pixelShader->getFilePath());
	pipeline->fShader = Renderer::compileShader(fragShaderSrc, GL_FRAGMENT_SHADER);
	if (pipeline->fShader == 0) {
		Renderer::errorShaderCompile("Fragment", pipeline->fShader);
		pipeline->isReady = false;
	}

	// Shader Program Creation and Linking
	GLint result;

	pipeline->shaderProg = glCreateProgram();
	glAttachShader(pipeline->shaderProg, pipeline->vShader);
	if (tessCtrlShader != nullptr) glAttachShader(pipeline->shaderProg, pipeline->tcShader);
	if(tessEvalShader != nullptr) glAttachShader(pipeline->shaderProg, pipeline->teShader);
	if(geomShader != nullptr) glAttachShader(pipeline->shaderProg, pipeline->gShader);
	glAttachShader(pipeline->shaderProg, pipeline->fShader);
	glLinkProgram(pipeline->shaderProg);

	glGetProgramiv(pipeline->shaderProg, GL_LINK_STATUS, &result);
	if (result == GL_FALSE){
		Renderer::errorProgramLink(pipeline->shaderProg);
		pipeline->isReady = false;
	}
	else { // detach after successful link
		glDetachShader(pipeline->shaderProg, pipeline->vShader);
		glDetachShader(pipeline->shaderProg, pipeline->fShader);
	}

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}