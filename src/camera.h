/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
 public:
	Camera();
	virtual ~Camera();

	glm::mat4 GetViewMatrix() const;
	void Rotate(float yaw, float pitch); // rotate camera on XY axis ... in degrees
	void SetLookAt(const glm::vec3& target);
	void SetRadius(float radius);
	const glm::vec3& GetPosition() const;

 private:

	void UpdateCameraVectors();

	glm::vec3 position_;
	glm::vec3 target_position_;
	glm::vec3 up_orientation_;

	// Eulers angles (in radians)
	float yaw_;
	float pitch_;
	float radius_;
};

#endif // CAMERA_H_
