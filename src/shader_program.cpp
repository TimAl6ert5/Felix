/*
 *    Due Date: 2020-11-15
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 3-2
 */

#include "shader_program.h"

ShaderProgram::ShaderProgram()
	: program_handle_(0)
{}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(program_handle_);
}

bool ShaderProgram::LoadShadersFiles(const char* vsFilename, const char* fsFilename) {
	std::string vsString = fileToString(vsFilename);
	std::string fsString = fileToString(fsFilename);
	return LoadShaders(vsString.c_str(), fsString.c_str());
}

bool ShaderProgram::LoadShaders(const GLchar* vsSourcePtr, const GLchar* fsSourcePtr) {

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);

	glCompileShader(vs);
	CheckCompileErrors(vs, kVertex);

	glCompileShader(fs);
	CheckCompileErrors(fs, kFragment);

	// Create the shader program
	program_handle_ = glCreateProgram();
	glAttachShader(program_handle_, vs);
	glAttachShader(program_handle_, fs);
	glLinkProgram(program_handle_);

	CheckCompileErrors(program_handle_, kProgram);

	// House keeping (don't leak memory) - we already compiled and linked the shaders, so delete them.
	glDeleteShader(vs);
	glDeleteShader(fs);

	uniform_locations_.clear();
	return true;
}

void ShaderProgram::Use() {
	if (program_handle_ > 0) {
		glUseProgram(program_handle_);
	}
}

void ShaderProgram::CheckCompileErrors(GLuint shader, ShaderType type) {
	int status = 0;

	if (type == kProgram) {
		glGetProgramiv(program_handle_, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length = 0;
			glGetProgramiv(program_handle_, GL_INFO_LOG_LENGTH, &length);

			std::string errorLog(length, ' ');
			glGetProgramInfoLog(program_handle_, length, &length, &errorLog[0]);
			std::cerr << "Error! Program failed to link. " << errorLog << std::endl;
		}
	} else {
		// type == kVertex || kFragment
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			std::string errorLog(length, ' ');
			glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
			std::cerr << "Error! Shader failed to compile. " << errorLog << std::endl;
		}
	}
}

std::string ShaderProgram::fileToString(const std::string& filename) {
	std::stringstream ss;
	std::ifstream file;

	try {
		file.open(filename, std::ios::in);

		if (!file.fail()) {
			ss << file.rdbuf();
		}

		file.close();
	} catch (...) {
		std::cerr << "Error reading shader filename!" << std::endl;
	}

	return ss.str();
}

GLuint ShaderProgram::GetProgram() const {
	return program_handle_;
}

GLint ShaderProgram::GetUniformLocation(const GLchar* name) {
	std::map<std::string, GLint>::iterator it = uniform_locations_.find(name);

	if (it == uniform_locations_.end()) {
		uniform_locations_[name] = glGetUniformLocation(program_handle_, name);
	}

	return uniform_locations_[name];
}

void ShaderProgram::SetUniform(const GLchar* name, const glm::vec2& v) {
	GLint loc = GetUniformLocation(name);
	glUniform2f(loc, v.x, v.y);
}

void ShaderProgram::SetUniform(const GLchar* name, const glm::vec3& v) {
	GLint loc = GetUniformLocation(name);
	glUniform3f(loc, v.x, v.y, v.z);
}

void ShaderProgram::SetUniform(const GLchar* name, const glm::vec4& v) {
	GLint loc = GetUniformLocation(name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void ShaderProgram::SetUniform(const GLchar* name, const glm::mat4& m) {
	GLint loc = GetUniformLocation(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::SetUniform(const GLchar* name, const GLfloat f) {
	GLint loc = GetUniformLocation(name);
	glUniform1f(loc, f);
}

void ShaderProgram::SetUniform(const GLchar* name, const GLint v) {
	GLint loc = GetUniformLocation(name);
	glUniform1i(loc, v);
}

void ShaderProgram::SetUniformSampler(const GLchar* name, const GLint& slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	GLint loc = GetUniformLocation(name);
	glUniform1i(loc, slot);
}

