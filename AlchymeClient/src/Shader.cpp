#include "Shader.hpp"
#include <easylogging++.h>
#include <RmlUi/Core.h>

Shader::Shader(const char* vertPath, const char* fragPath) {

	throw std::runtime_error("Not supported");

	// Load file
	std::string vertCode;
	std::string fragCode;

	Rml::GetFileInterface()->LoadFile(vertPath, vertCode);
	Rml::GetFileInterface()->LoadFile(fragPath, fragCode);

	// Compile
	unsigned int vert, frag;
	int success;
	char infoLog[512];

	// VERTEX SHADER
	vert = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vert, 1, vertCode.c_str(), NULL);
	glCompileShader(vert);

	// errors
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
	if (!success) {
		//glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		throw std::runtime_error(std::string("vert shader compile error: ") + infoLog);
	}

	// FRAGMENT SHADER
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(frag, 1, &fragCode.c_str(), NULL);
	glCompileShader(frag);

	// errors
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(frag, 512, NULL, infoLog);
		throw std::runtime_error(std::string("frag shader compile error: ") + infoLog);
	}

	// Shader prog
	id = glCreateProgram();
	glAttachShader(id, vert);
	glAttachShader(id, frag);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		throw std::runtime_error(std::string("shader link error: ") + infoLog);
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
}

void Shader::Enable() {
	glUseProgram(id);
}

void Shader::SetBool(const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
