/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderProgram {
 public:
	ShaderProgram();
	virtual ~ShaderProgram();

	enum ShaderType {
		kVertex,
		kFragment,
		kProgram
	};

	bool LoadShadersFiles(const std::string& vsFilename, const std::string& fsFilename);
	bool LoadShaders(const GLchar* vsSourcePtr, const GLchar* fsSourcePtr);
	void Use();

	void SetUniform(const GLchar* name, const glm::vec2& v);
	void SetUniform(const GLchar* name, const glm::vec3& v);
	void SetUniform(const GLchar* name, const glm::vec4& v);
	void SetUniform(const GLchar* name, const glm::mat4& m);
	void SetUniform(const GLchar* name, const GLfloat f);
	void SetUniform(const GLchar* name, const GLint v);
	void SetUniformSampler(const GLchar* name, const GLint& slot);

	GLuint GetProgram() const;

 private:

	void CheckCompileErrors(GLuint shader, ShaderType type);
	GLint GetUniformLocation(const GLchar* name);
	std::string fileToString(const std::string& filename);

	GLuint program_handle_;
	std::map<std::string, GLint> uniform_locations_;

};

#endif // SHADER_PROGRAM_H_
