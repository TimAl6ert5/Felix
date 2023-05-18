/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <glm/glm.hpp>

#include "camera.h"
#include "shader_program.h"
#include "mesh.h"
#include "texture2D.h"

class Scene {
 public:
	void SetSubjectFiles(std::string mesh_filename, std::string texture_filename);

	void LoadShaders();

	bool LoadSubjectMesh();
	bool LoadKeyLightMesh();
	bool LoadFillLightMesh();

	void InitMeshBuffers();

	bool LoadSubjectTexture();

	void RenderScene(GLint window_width, GLint window_height);

	void SetOrthographic();
	void SetPerspective();

	void SetCameraYaw(float yaw);
	void SetCameraPitch(float pitch);
	void SetCameraRadius(float radius);

 private:
	glm::mat4 TransformCamera(float camera_yaw, float camera_pitch, float camera_radius);

	void RenderSubject(glm::mat4 view, glm::mat4 projection);
	glm::mat4 TransformSubject() const;

	void RenderKeyLight(glm::mat4 view, glm::mat4 projection);
	glm::mat4 TransformKeyLight() const;

	void RenderFillLight(glm::mat4 view, glm::mat4 projection);
	glm::mat4 TransformFillLight() const;

	bool mode_perspective_ = true;

	float camera_yaw_ = 0.0f;
	float camera_pitch_ = 0.0f;
	float camera_radius_ = 12.0f;

	glm::vec3 camera_position_ = glm::vec3(0.0f, 0.0f, -6.0f);
	float camera_rotation_ = glm::radians(-25.0f);
	Camera orbit_camera_;

	glm::vec3 subject_position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 subject_scale_ = glm::vec3(0.5f);

	std::string subject_mesh_filename_;
	std::string subject_texture_filename_;

	ShaderProgram subject_shader_;

	const std::string kSubjectVertexShaderSrc_ = "shader_subject.vert";
	const std::string kSubjectFragmentShaderSrc_ = "shader_subject.frag";

	Mesh subject_mesh_;
	Texture2D subject_texture_;

	glm::vec3 key_light_position_ = glm::vec3(4.0f, 4.0f, -4.0f);
	glm::vec3 key_light_scale_ = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 key_light_color_ = glm::vec3(1.0f, 1.0f, 1.0f); // White
	float key_light_intensity_ = 1.0f;

	glm::vec3 fill_light_position_ = glm::vec3(-4.0f, 1.0f, 4.0f);
	glm::vec3 fill_light_scale_ = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 fill_light_color_ = glm::vec3(0.8f, 0.8f, 1.0f); // Light Blue
	float fill_light_intensity_ = 0.3f;

	ShaderProgram lamp_shader_;

	const std::string kLightVertexShaderSrc_ = "shader_lighting.vert";
	const std::string kLightFragmentShaderSrc_ = "shader_lighting.frag";

	const std::string kLampMeshFilename_ = "LightSphere.obj";

	Mesh key_light_mesh_;
	Mesh fill_light_mesh_;

};

#endif // SCENE_H_
