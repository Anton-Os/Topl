#include "Topl_Renderer_GL4.hpp"

namespace _GL4 {
	void errorNotFound(const char* shaderName){
		printf("%s not found", shaderName);
	}

	void errorShaderCompile(const char* shaderName, GLuint shader){
		printf("%s shader compilation failed", shaderName); // Add more robust error checking

		GLint maxLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetShaderInfoLog(shader, maxLen, &maxLen, errorLog);
		puts(errorLog);
		free(errorLog);

		return;
	}

	void errorProgramLink(GLuint shaderProg){
		puts("Shader program failed to link");

		GLint maxLen;
		glGetProgramiv(shaderProg, GL_INFO_LOG_LENGTH, &maxLen);

		char* errorLog = (char*)malloc(maxLen * sizeof(char));
		glGetProgramInfoLog(shaderProg, maxLen, &maxLen, errorLog);
		puts(errorLog);
		free(errorLog);

		return;
	}
}

void Topl_Renderer_GL4::setPipeline(Topl_Pipeline_GL4* pipeline){
	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);
	
	if(_isPipelineReady) glUseProgram(pipeline->shaderProg);
}

void Topl_Renderer_GL4::genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr fragShader){
	GLint result;
	const char* source;

	// vertex shader creation and valid file checking
	pipeline->vShader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertexShaderSrc = readFile(vertexShader->getFilePath(), false);
	source = vertexShaderSrc.c_str();
	if (!vertexShaderSrc.empty()) glShaderSource(pipeline->vShader, 1, &source, NULL);
	else {
		_GL4::errorNotFound("Vertex");
		pipeline->isReady = false;
	}

	// vertex shader compilation and syntax checking
	glCompileShader(pipeline->vShader);
	glGetShaderiv(pipeline->vShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
		_GL4::errorShaderCompile("Vertex", pipeline->vShader);
		pipeline->isReady = false;
	}


	// fragment shader creation and valid file checking
	pipeline->fShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragShaderSrc = readFile(fragShader->getFilePath(), false);
	source = fragShaderSrc.c_str();
	if (!fragShaderSrc.empty()) glShaderSource(pipeline->fShader, 1, &source, NULL);
	else {
		_GL4::errorNotFound("Fragment");
		pipeline->isReady = false;
	}

	// fragment shader compilation and syntax checking
	glCompileShader(pipeline->fShader);
	glGetShaderiv(pipeline->fShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
		_GL4::errorShaderCompile("Fragment", pipeline->fShader);
		pipeline->isReady = false;
	}


	// program creation code
	pipeline->shaderProg = glCreateProgram();
	glAttachShader(pipeline->shaderProg, pipeline->vShader);
	glAttachShader(pipeline->shaderProg, pipeline->fShader);
	glLinkProgram(pipeline->shaderProg);

	glGetProgramiv(pipeline->shaderProg, GL_LINK_STATUS, &result);
	if (result == GL_FALSE){
		_GL4::errorProgramLink(pipeline->shaderProg);
		pipeline->isReady = false;
	}
	else {
		// always detach after successful link
		glDetachShader(pipeline->shaderProg, pipeline->vShader);
		glDetachShader(pipeline->shaderProg, pipeline->fShader);
	}

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}

void Topl_Renderer_GL4::genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr fragShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
	GLint result;
	const char* source;

	// vertex shader creation and valid file checking
	pipeline->vShader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertexShaderSrc = readFile(vertexShader->getFilePath(), false);
	source = vertexShaderSrc.c_str();
	if (!vertexShaderSrc.empty()) glShaderSource(pipeline->vShader, 1, &source, NULL);
	else {
		_GL4::errorNotFound("Vertex");
		pipeline->isReady = false;
	}

	// vertex shader compilation and syntax checking
	glCompileShader(pipeline->vShader);
	glGetShaderiv(pipeline->vShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
		_GL4::errorShaderCompile("Vertex", pipeline->vShader);
		pipeline->isReady = false;
	}


	// tesselation control shader creation and valid file checking
	pipeline->tcShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	std::string tessCtrlShaderSrc = readFile(tessCtrlShader->getFilePath(), false);
	source = tessCtrlShaderSrc.c_str();
	if (!tessCtrlShaderSrc.empty()) glShaderSource(pipeline->tcShader, 1, &source, NULL);
	else {
		_GL4::errorNotFound("Tess Control");
		pipeline->isReady = false;
	}

	// tesselation control compilation and syntax checking
	glCompileShader(pipeline->tcShader);
	glGetShaderiv(pipeline->tcShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
		_GL4::errorShaderCompile("Tess Control", pipeline->tcShader);
		pipeline->isReady = false;
	}


	// tesselation evaluation shader creation and valid file checking
	pipeline->teShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	std::string tessEvalShaderSrc = readFile(tessEvalShader->getFilePath(), false);
	source = tessEvalShaderSrc.c_str();
	if (!tessEvalShaderSrc.empty()) glShaderSource(pipeline->teShader, 1, &source, NULL);
	else {
		_GL4::errorNotFound("Tess Evaluation");
		pipeline->isReady = false;
	}

	// tesselation evaluation compilation and syntax checking
	glCompileShader(pipeline->teShader);
	glGetShaderiv(pipeline->teShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		_GL4::errorShaderCompile("Tess Evaluation", pipeline->teShader);
		pipeline->isReady = false;
	}


	// geometry shader creation and valid file checking
	pipeline->gShader = glCreateShader(GL_GEOMETRY_SHADER);
	std::string geomShaderSrc = readFile(geomShader->getFilePath(), false);
	source = geomShaderSrc.c_str();
	if (!geomShaderSrc.empty()) glShaderSource(pipeline->gShader, 1, &source, NULL);
	else {
		_GL4::errorNotFound("Geometry");
		pipeline->isReady = false;
	}

	// geometry shader compilation and syntax checking
	glCompileShader(pipeline->gShader);
	glGetShaderiv(pipeline->gShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		_GL4::errorShaderCompile("Geometry", pipeline->gShader);
		pipeline->isReady = false;
	}


	// fragment shader creation and valid file checking
	pipeline->fShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragShaderSrc = readFile(fragShader->getFilePath(), false);
	source = fragShaderSrc.c_str();
	if (!fragShaderSrc.empty()) glShaderSource(pipeline->fShader, 1, &source, NULL);
	else {
		_GL4::errorNotFound("Fragment");
		pipeline->isReady = false;
	}

	// fragment shader compilation and syntax checking
	glCompileShader(pipeline->fShader);
	glGetShaderiv(pipeline->fShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
		_GL4::errorShaderCompile("Fragment", pipeline->fShader);
		pipeline->isReady = false;
	}


	// program creation code
	pipeline->shaderProg = glCreateProgram();
	glAttachShader(pipeline->shaderProg, pipeline->vShader);
	glAttachShader(pipeline->shaderProg, pipeline->fShader);
	glLinkProgram(pipeline->shaderProg);

	glGetProgramiv(pipeline->shaderProg, GL_LINK_STATUS, &result);
	if (result == GL_FALSE){
		_GL4::errorProgramLink(pipeline->shaderProg);
		pipeline->isReady = false;
	}
	else {
		// always detach after successful link
		glDetachShader(pipeline->shaderProg, pipeline->vShader);
		glDetachShader(pipeline->shaderProg, pipeline->fShader);
	}

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}