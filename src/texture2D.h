/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#ifndef TEXTURE_2D_H_
#define TEXTURE_2D_H_

#include <iostream>
#include <GL/glew.h>
#include "SOIL2/SOIL2.h"

class Texture2D {
 public:
	Texture2D();

	bool LoadTexture(const char* filename);
	void Bind(GLuint texUnit = 0);
	void Unbind(GLuint texUnit = 0);

 private:
	GLuint texture_;

};

#endif // TEXTURE_2D_H_
