/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#ifndef MESH_H_
#define MESH_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh {
 public:
	Mesh();
	virtual ~Mesh();

	bool LoadOBJ(const std::string& filename);
	void InitBuffers();
	void Draw();

	float GetXMin() const;
	float GetXMax() const;

	float GetYMin() const;
	float GetYMax() const;

	float GetZMin() const;
	float GetZMax() const;

 private:
	void UpdateDimensions(glm::vec3 vertex, bool first);

	bool file_loaded_;
	bool buffers_initialized_;
	std::vector<Vertex> vertices_;
	GLuint vbo_, vao_;

	float x_min_, x_max_, y_min_, y_max_, z_min_, z_max_;
};

#endif // MESH_H_
