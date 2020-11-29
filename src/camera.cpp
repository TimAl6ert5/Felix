/*
 *    Due Date: 2020-11-15
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 3-2
 */

#include "camera.h"

Camera::Camera()
	:position_(glm::vec3(0.0f, 0.0f, 0.0f)),
	target_position_(glm::vec3(0.0f, 0.0f, 0.0f)),
	up_orientation_(0.0f, 1.0f, 0.0f), // Y is up
	yaw_(0.0f),
	pitch_(0.0f),
	radius_(0.0f)
{}

Camera::~Camera()
{}

glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(position_, target_position_, up_orientation_);
}

void Camera::SetLookAt(const glm::vec3& target) {
	target_position_ = target;
}

void Camera::SetRadius(float radius) {
	radius_ = glm::clamp(radius, 2.0f, 80.0f);
}

const glm::vec3& Camera::GetPosition() const {
	return position_;
}

void Camera::Rotate(float yaw, float pitch) {
	yaw_ = glm::radians(yaw);
	pitch_ = glm::radians(pitch);

	pitch_ = glm::clamp(pitch_, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);
	UpdateCameraVectors();
}

void Camera::UpdateCameraVectors() {
	// Spherical to Cartesian coordinates
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system (Note: our coordinate system has Y up not Z)
	position_.x = target_position_.x + radius_ * cosf(pitch_) * sinf(yaw_);
	position_.y = target_position_.y + radius_ * sinf(pitch_);
	position_.z = target_position_.z + radius_ * cosf(pitch_) * cosf(yaw_);
}

