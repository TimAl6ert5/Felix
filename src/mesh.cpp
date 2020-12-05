/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#include "mesh.h"


/* Split a string on a delimiter*/
std::vector<std::string> split(std::string s, std::string t) {
	std::vector<std::string> res;
	while (1) {
		int pos = s.find(t);
		if (pos == -1) {
			res.push_back(s);
			break;
		}
		res.push_back(s.substr(0, pos));
		s = s.substr(pos + 1, s.size() - pos - 1);
	}
	return res;
}


Mesh::Mesh()
	:file_loaded_(false),
	 buffers_initialized_(false)
{}

Mesh::~Mesh() {
	// Delete Buffer objects only if they were initialized
	if (buffers_initialized_) {
		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(1, &vbo_);
	}
}

bool Mesh::LoadOBJ(const std::string& filename) {
	std::vector<unsigned int> vertexIndices, normalIndices, uvIndices;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec3> tempNormals;
	std::vector<glm::vec2> tempUVs;
	bool first = true;

	if (filename.find(".obj") != std::string::npos) {
		std::ifstream fin(filename, std::ios::in);
		if (!fin) {
			std::cerr << "Cannot open " << filename << std::endl;
			return false;
		}

		std::cout << "Loading OBJ file " << filename << " ..." << std::endl;

		std::string lineBuffer;
		while (std::getline(fin, lineBuffer)) {
			std::stringstream ss(lineBuffer);
			std::string cmd;
			ss >> cmd;

			if (cmd == "v") {
				glm::vec3 vertex;
				int dim = 0;
				while (dim < 3 && ss >> vertex[dim])
					dim++;

				tempVertices.push_back(vertex);
				UpdateDimensions(vertex, first);
				first = false;
			}
			else if (cmd == "vt") {
				glm::vec2 uv; // ignores the third component 'w'
				int dim = 0;
				while (dim < 2 && ss >> uv[dim])
					dim++;

				tempUVs.push_back(uv);
			}
			else if (cmd == "vn") {
				glm::vec3 normal;
				int dim = 0;
				while (dim < 3 && ss >> normal[dim])
					dim++;

				normal = glm::normalize(normal); // normalize the normal for magnitude of 1
				tempNormals.push_back(normal);
			}
			else if (cmd == "f") {
				// Note: only loads triangles ... will fail if there are quads
				// does allow for faces to not have texture

				std::string faceData;
				int vertexIndex, uvIndex, normalIndex; /* v/vt/vn */

				while (ss >> faceData) {
					std::vector<std::string> data = split(faceData, "/");

					// vertex index
					if (data[0].size() > 0) {
						sscanf_s(data[0].c_str(), "%d", &vertexIndex);
						vertexIndices.push_back(vertexIndex);
					}

					// if the face vertex has a texture coordinate index
					if (data.size() >= 1) {
						if (data[1].size() > 0) {
							sscanf_s(data[1].c_str(), "%d", &uvIndex);
							uvIndices.push_back(uvIndex);
						}
					}

					// does the face vertex have a normal index
					if (data.size() >= 2) {
						if (data[2].size() > 0) {
							sscanf_s(data[2].c_str(), "%d", &normalIndex);
							normalIndices.push_back(normalIndex);
						}
					}
				}
			}
		}

		// Close the file
		fin.close();


		// For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			Vertex meshVertex;

			if (tempVertices.size() > 0) {
				glm::vec3 vertex = tempVertices[vertexIndices[i] - 1];
				meshVertex.position = vertex;
			}

			if (tempNormals.size() > 0) {
				glm::vec3 normal = tempNormals[normalIndices[i] - 1];
				meshVertex.normal = normal;
			}

			if (tempUVs.size() > 0) {
				glm::vec2 uv = tempUVs[uvIndices[i] - 1];
				meshVertex.texCoords = uv;
			}

			vertices_.push_back(meshVertex);
		}

		return (file_loaded_ = true);
	}

	// Shouldn't get here so return failure
	return false;
}

void Mesh::UpdateDimensions(glm::vec3 vertex, bool first) {
	if(first) {
		// Initialize all min/max dimensions
		x_min_ = x_max_ = vertex[0];
		y_min_ = y_max_ = vertex[1];
		z_min_ = z_max_ = vertex[2];
	} else {
		// Update all min/max dimensions
		x_min_ = std::min(x_min_, vertex[0]);
		x_max_ = std::max(x_max_, vertex[0]);

		y_min_ = std::min(y_min_, vertex[1]);
		y_max_ = std::max(y_max_, vertex[1]);

		z_min_ = std::min(z_min_, vertex[2]);
		z_max_ = std::max(z_max_, vertex[2]);
	}
}

float Mesh::GetXMin() const {
	return x_min_;
}

float Mesh::GetXMax() const {
	return x_max_;
}

float Mesh::GetYMin() const {
	return y_min_;
}

float Mesh::GetYMax() const {
	return y_max_;
}

float Mesh::GetZMin() const {
	return z_min_;
}

float Mesh::GetZMax() const {
	return z_max_;
}

void Mesh::Draw() {
	if(!file_loaded_) return;

	glBindVertexArray(vao_);
	glDrawArrays(GL_TRIANGLES, 0, vertices_.size());

	glBindVertexArray(0); // 'unbind'
}

void Mesh::InitBuffers() {
	glGenBuffers(1, &vbo_); // create one vbo and save the identifier in vbo ... creates a chunk of memory in the graphics card
	glBindBuffer(GL_ARRAY_BUFFER, vbo_); // define the type of buffer and give it an identifier ... makes this buffer the current buffer
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW); // give the buffer type, size and content along with how to treat the buffer (GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW) ... meaning how often the data will be updated

	glGenVertexArrays(1, &vao_); // create 1 vao and store identifier
	glBindVertexArray(vao_); // make it the active one

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); // tell the graphics card how to interpret the buffer data
	glEnableVertexAttribArray(0); // enable the vao

	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // enable the vao

	// tex coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2); // enable the vao

	glBindVertexArray(0); // 'unbind'
	buffers_initialized_ = true;
}

