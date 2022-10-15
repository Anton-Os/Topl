#include "opengl/Topl_Renderer_GL4.hpp"

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
		if (result == GL_FALSE) return shader; // return 0;

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
	pipeline->vertexShader = GL4::compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
	if (pipeline->vertexShader == 0) {
		GL4::errorShaderCompile("Vertex", pipeline->vertexShader);
		pipeline->isReady = false;
	}

	// Fragment Shader

	std::string fragShaderSrc = readFile(pixelShader->getFilePath());
	pipeline->fragmentShader = GL4::compileShader(fragShaderSrc, GL_FRAGMENT_SHADER);
	if (pipeline->fragmentShader == 0) {
		GL4::errorShaderCompile("Fragment", pipeline->fragmentShader);
		pipeline->isReady = false;
	}

	// Shader Program Creation and Linking
	GLint result;

	pipeline->shaderProg = glCreateProgram();
	glAttachShader(pipeline->shaderProg, pipeline->vertexShader);
	glAttachShader(pipeline->shaderProg, pipeline->fragmentShader);
	glLinkProgram(pipeline->shaderProg);

	glGetProgramiv(pipeline->shaderProg, GL_LINK_STATUS, &result);
	if (result == GL_FALSE){
		GL4::errorProgramLink(pipeline->shaderProg);
		pipeline->isReady = false;
	}
	else { // detach after successful link
		glDetachShader(pipeline->shaderProg, pipeline->vertexShader);
		glDetachShader(pipeline->shaderProg, pipeline->fragmentShader);
	}

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}

void Topl_Renderer_GL4::genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader){
	// Vertex Shader

	std::string vertexShaderSrc = readFile(vertexShader->getFilePath());
	pipeline->vertexShader = GL4::compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
	if (pipeline->vertexShader == 0) {
		GL4::errorShaderCompile("Vertex", pipeline->vertexShader);
		pipeline->isReady = false;
	}

	// Fragment Shader

	std::string fragShaderSrc = readFile(pixelShader->getFilePath());
	pipeline->fragmentShader = GL4::compileShader(fragShaderSrc, GL_FRAGMENT_SHADER);
	if (pipeline->fragmentShader == 0) {
		GL4::errorShaderCompile("Fragment", pipeline->fragmentShader);
		pipeline->isReady = false;
	}

	// Geometry Shader

	if (geomShader != nullptr) { // optional stage
		std::string geomShaderSrc = readFile(geomShader->getFilePath());
		pipeline->geomShader = GL4::compileShader(geomShaderSrc, GL_GEOMETRY_SHADER);
		if (pipeline->geomShader == 0) {
			GL4::errorShaderCompile("Geometry", pipeline->geomShader);
			pipeline->isReady = false;
		}
	}

	// Tess. Control Shader

	if(tessCtrlShader != nullptr){ // optional stage
		std::string tessCtrlShaderSrc = readFile(tessCtrlShader->getFilePath());
		pipeline->tessCtrlShader = GL4::compileShader(tessCtrlShaderSrc, GL_TESS_CONTROL_SHADER);
		if (pipeline->tessCtrlShader == 0) {
			GL4::errorShaderCompile("Tess. Control", pipeline->tessCtrlShader);
			pipeline->isReady = false;
		}
	}

	// Tess. Evaluation Shader

	if(tessEvalShader != nullptr){ // optional stage
		std::string tessEvalShaderSrc = readFile(tessEvalShader->getFilePath());
		pipeline->tessEvalShader = GL4::compileShader(tessEvalShaderSrc, GL_TESS_EVALUATION_SHADER);
		if (pipeline->tessEvalShader == 0) {
			GL4::errorShaderCompile("Tess. Eval", pipeline->tessEvalShader);
			pipeline->isReady = false;
		}
	}

	// Shader Program Creation and Linking
	GLint result;

	pipeline->shaderProg = glCreateProgram();
	glAttachShader(pipeline->shaderProg, pipeline->vertexShader);
	if (tessCtrlShader != nullptr) glAttachShader(pipeline->shaderProg, pipeline->tessCtrlShader);
	if(tessEvalShader != nullptr) glAttachShader(pipeline->shaderProg, pipeline->tessEvalShader);
	if(geomShader != nullptr) glAttachShader(pipeline->shaderProg, pipeline->geomShader);
	glAttachShader(pipeline->shaderProg, pipeline->fragmentShader);
	glLinkProgram(pipeline->shaderProg);

	glGetProgramiv(pipeline->shaderProg, GL_LINK_STATUS, &result);
	if (result == GL_FALSE){
		GL4::errorProgramLink(pipeline->shaderProg);
		pipeline->isReady = false;
	}
	else { // detach after successful link
		glDetachShader(pipeline->shaderProg, pipeline->vertexShader);
		glDetachShader(pipeline->shaderProg, pipeline->fragmentShader);
	}

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}