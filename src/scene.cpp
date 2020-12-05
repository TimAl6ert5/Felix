/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#include "scene.h"

void Scene::RenderScene(GLint window_width, GLint window_height) {

	glm::mat4 model, view, projection;

	// Transform the camera
	view = TransformCamera(camera_yaw, camera_pitch, camera_radius);

	// Create perspective or orthographic projection
	if(mode_perspective) {
		projection = glm::perspective(glm::radians(45.0f), (GLfloat)window_width / (GLfloat)window_height, 0.1f, 100.0f);
	} else {
		projection = glm::ortho(-5.0f,5.0f,-5.0f,5.0f,0.1f,100.0f);
	}

	RenderSubject(view, projection);
	RenderKeyLight(view, projection);
	RenderFillLight(view, projection);
}

void Scene::SetCameraYaw(float yaw) {
	camera_yaw -= yaw;
}

void Scene::SetCameraPitch(float pitch) {
	camera_pitch += pitch;
}

void Scene::SetCameraRadius(float radius) {
	camera_radius += radius;
}

void Scene::SetOrthographic() {
	mode_perspective = false;
}

void Scene::SetPerspective() {
	mode_perspective = true;
}

glm::mat4 Scene::TransformCamera(float camera_yaw, float camera_pitch, float camera_radius) {
	orbit_camera.SetLookAt(subject_position);
	orbit_camera.Rotate(camera_yaw, camera_pitch);
	orbit_camera.SetRadius(camera_radius);
	return orbit_camera.GetViewMatrix();
}

void Scene::RenderSubject(glm::mat4 view, glm::mat4 projection) {
	glm::mat4 model = TransformSubject();

	subject_shader.Use();

	subject_shader.SetUniform("view", view);
	subject_shader.SetUniform("projection", projection);
	subject_shader.SetUniform("viewPosition", orbit_camera.GetPosition());

	subject_shader.SetUniform("lights[0].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	subject_shader.SetUniform("lights[0].diffuse", key_light_color);
	subject_shader.SetUniform("lights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));// white
	subject_shader.SetUniform("lights[0].position", key_light_position);
	subject_shader.SetUniform("lights[0].intensity", key_light_intensity);

	subject_shader.SetUniform("lights[1].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	subject_shader.SetUniform("lights[1].diffuse", fill_light_color);
	subject_shader.SetUniform("lights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));// white
	subject_shader.SetUniform("lights[1].position", fill_light_position);
	subject_shader.SetUniform("lights[1].intensity", fill_light_intensity);

	subject_shader.SetUniform("model", model);

	subject_shader.SetUniform("material.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	subject_shader.SetUniformSampler("material.diffuseMap", 0);
	subject_shader.SetUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	subject_shader.SetUniform("material.shininess", 64.0f);

	subject_texture.Bind(0);
	subject_mesh.Draw();
	subject_texture.Unbind(0);
}

glm::mat4 Scene::TransformSubject() const {
	glm::mat4 model = glm::translate(glm::mat4(), subject_position);
	model = glm::scale(model, subject_scale);
	return model;
}

void Scene::RenderKeyLight(glm::mat4 view, glm::mat4 projection) {
	glm::mat4 model = TransformKeyLight();
	lamp_shader.Use();
	lamp_shader.SetUniform("lampColor", key_light_color);
	lamp_shader.SetUniform("model", model);
	lamp_shader.SetUniform("view", view);
	lamp_shader.SetUniform("projection", projection);
	key_light_mesh.Draw();
}

glm::mat4 Scene::TransformKeyLight() const {
	glm::mat4 model = glm::translate(glm::mat4(), key_light_position);
	model = glm::scale(model, key_light_scale);
	return model;
}

void Scene::RenderFillLight(glm::mat4 view, glm::mat4 projection) {
	glm::mat4 model = TransformFillLight();
	lamp_shader.Use();
	lamp_shader.SetUniform("lampColor", fill_light_color);
	lamp_shader.SetUniform("model", model);
	lamp_shader.SetUniform("view", view);
	lamp_shader.SetUniform("projection", projection);
	fill_light_mesh.Draw();
}

glm::mat4 Scene::TransformFillLight() const {
	glm::mat4 model = glm::translate(glm::mat4(), fill_light_position);
	model = glm::scale(model, fill_light_scale);
	return model;
}

void Scene::LoadShaders() {
	subject_shader.LoadShadersFiles(kSubjectVertexShaderSrc, kSubjectFragmentShaderSrc);
	lamp_shader.LoadShadersFiles(kLightVertexShaderSrc, kLightFragmentShaderSrc);
}

bool Scene::LoadSubjectMesh() {
	return subject_mesh.LoadOBJ(subject_mesh_filename);
}

bool Scene::LoadSubjectTexture() {
	subject_texture.LoadTexture(subject_texture_filename.c_str());
	// TODO verify the texture loaded
	return true;
}

bool Scene::LoadKeyLightMesh() {
	return key_light_mesh.LoadOBJ(kLampMeshFilename);
}

bool Scene::LoadFillLightMesh() {
	return fill_light_mesh.LoadOBJ(kLampMeshFilename);
}

void Scene::InitMeshBuffers() {
	subject_mesh.InitBuffers();
	key_light_mesh.InitBuffers();
	fill_light_mesh.InitBuffers();
}

void Scene::SetSubjectFiles(std::string mesh_filename, std::string texture_filename) {
	subject_mesh_filename = mesh_filename;
	subject_texture_filename = texture_filename;
}
