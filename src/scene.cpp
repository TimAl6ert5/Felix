/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#include "scene.h"

void Scene::RenderScene(GLint window_width, GLint window_height) {

	glm::mat4 view, projection;

	// Transform the camera
	view = TransformCamera(camera_yaw_, camera_pitch_, camera_radius_);

	// Create perspective or orthographic projection
	if(mode_perspective_) {
		projection = glm::perspective(glm::radians(45.0f), (GLfloat)window_width / (GLfloat)window_height, 0.1f, 100.0f);
	} else {
		projection = glm::ortho(-5.0f,5.0f,-5.0f,5.0f,0.1f,100.0f);
	}

	RenderSubject(view, projection);
	RenderKeyLight(view, projection);
	RenderFillLight(view, projection);
}

void Scene::SetCameraYaw(float yaw) {
	camera_yaw_ -= yaw;
}

void Scene::SetCameraPitch(float pitch) {
	camera_pitch_ += pitch;
}

void Scene::SetCameraRadius(float radius) {
	camera_radius_ += radius;
}

void Scene::SetOrthographic() {
	mode_perspective_ = false;
}

void Scene::SetPerspective() {
	mode_perspective_ = true;
}

glm::mat4 Scene::TransformCamera(float camera_yaw, float camera_pitch, float camera_radius) {
	orbit_camera_.SetLookAt(subject_position_);
	orbit_camera_.Rotate(camera_yaw, camera_pitch);
	orbit_camera_.SetRadius(camera_radius);
	return orbit_camera_.GetViewMatrix();
}

void Scene::RenderSubject(glm::mat4 view, glm::mat4 projection) {
	glm::mat4 model = TransformSubject();

	subject_shader_.Use();

	subject_shader_.SetUniform("view", view);
	subject_shader_.SetUniform("projection", projection);
	subject_shader_.SetUniform("viewPosition", orbit_camera_.GetPosition());

	subject_shader_.SetUniform("lights[0].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	subject_shader_.SetUniform("lights[0].diffuse", key_light_color_);
	subject_shader_.SetUniform("lights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));// white
	subject_shader_.SetUniform("lights[0].position", key_light_position_);
	subject_shader_.SetUniform("lights[0].intensity", key_light_intensity_);

	subject_shader_.SetUniform("lights[1].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	subject_shader_.SetUniform("lights[1].diffuse", fill_light_color_);
	subject_shader_.SetUniform("lights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));// white
	subject_shader_.SetUniform("lights[1].position", fill_light_position_);
	subject_shader_.SetUniform("lights[1].intensity", fill_light_intensity_);

	subject_shader_.SetUniform("model", model);

	subject_shader_.SetUniform("material.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	subject_shader_.SetUniformSampler("material.diffuseMap", 0);
	subject_shader_.SetUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	subject_shader_.SetUniform("material.shininess", 64.0f);

	subject_texture_.Bind(0);
	subject_mesh_.Draw();
	subject_texture_.Unbind(0);
}

glm::mat4 Scene::TransformSubject() const {
	glm::mat4 model = glm::translate(glm::mat4(), subject_position_);
	model = glm::scale(model, subject_scale_);
	return model;
}

void Scene::RenderKeyLight(glm::mat4 view, glm::mat4 projection) {
	glm::mat4 model = TransformKeyLight();
	lamp_shader_.Use();
	lamp_shader_.SetUniform("lampColor", key_light_color_);
	lamp_shader_.SetUniform("model", model);
	lamp_shader_.SetUniform("view", view);
	lamp_shader_.SetUniform("projection", projection);
	key_light_mesh_.Draw();
}

glm::mat4 Scene::TransformKeyLight() const {
	glm::mat4 model = glm::translate(glm::mat4(), key_light_position_);
	model = glm::scale(model, key_light_scale_);
	return model;
}

void Scene::RenderFillLight(glm::mat4 view, glm::mat4 projection) {
	glm::mat4 model = TransformFillLight();
	lamp_shader_.Use();
	lamp_shader_.SetUniform("lampColor", fill_light_color_);
	lamp_shader_.SetUniform("model", model);
	lamp_shader_.SetUniform("view", view);
	lamp_shader_.SetUniform("projection", projection);
	fill_light_mesh_.Draw();
}

glm::mat4 Scene::TransformFillLight() const {
	glm::mat4 model = glm::translate(glm::mat4(), fill_light_position_);
	model = glm::scale(model, fill_light_scale_);
	return model;
}

void Scene::LoadShaders() {
	subject_shader_.LoadShadersFiles(kSubjectVertexShaderSrc_, kSubjectFragmentShaderSrc_);
	lamp_shader_.LoadShadersFiles(kLightVertexShaderSrc_, kLightFragmentShaderSrc_);
}

bool Scene::LoadSubjectMesh() {
	return subject_mesh_.LoadOBJ(subject_mesh_filename_);
}

bool Scene::LoadSubjectTexture() {
	subject_texture_.LoadTexture(subject_texture_filename_.c_str());
	// TODO verify the texture loaded
	return true;
}

bool Scene::LoadKeyLightMesh() {
	return key_light_mesh_.LoadOBJ(kLampMeshFilename_);
}

bool Scene::LoadFillLightMesh() {
	return fill_light_mesh_.LoadOBJ(kLampMeshFilename_);
}

void Scene::InitMeshBuffers() {
	subject_mesh_.InitBuffers();
	key_light_mesh_.InitBuffers();
	fill_light_mesh_.InitBuffers();
}

void Scene::SetSubjectFiles(std::string mesh_filename, std::string texture_filename) {
	subject_mesh_filename_ = mesh_filename;
	subject_texture_filename_ = texture_filename;
}
