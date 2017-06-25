//
// Created by Lauri Kortevaara on 26/12/15.
//

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ostream>
#include <iostream>
#include "Camera.h"
#include "Engine.h"
#include <math.h>
#include "component/JoystickInput.h"
#include "system/InputSystem.h"
#include "MathUtils.h"
#include "component/MouseInput.h"

using namespace omen;

/**
 *
 */
Camera::Camera(const std::string &name, const glm::vec3 &pos, const ::glm::vec3 &lookAt, omen::floatprec fov) :
	GameObject(name),
	m_pos(pos),
	m_near(1.0f), m_far(1000.0f),
	m_yaw(.0f), m_pitch(0.0f), m_roll(.0f),
	m_lookAt(lookAt),
	m_fov(fov*glm::pi<float>() / 180.0f),
	m_bIsValid(false),
	m_joystick(nullptr),
	m_sensorSize(DEFAULT_SENSOR_SIZE),
	m_focalLength(DEFAULT_FOCA_LENGTH)
{

	omen::Engine *e = omen::Engine::instance();
	//omen::Window *w = e->window();

	// Create View matrix
	//float aspect = (float)w->width() / (float)w->height();
	//m_view = glm::perspective(m_fov, aspect, m_near, m_far);

	// Initialize forward and up vectors
	m_forward = glm::vec3(0, 0, 1);
	m_up = glm::vec3(0, 1, 0);
	//m_pos.z *= -1;

	

	// connect to engine update signal
	Engine::instance()->signal_engine_update.connect(this,[this](omen::floatprec time, omen::floatprec deltaTime) {
		m_rotation = glm::rotate(glm::mat4(1), glm::radians(m_yaw), glm::vec3(0, 1, 0));
		m_rotation = glm::rotate(m_rotation, glm::radians(m_pitch), glm::vec3(1, 0, 0));
		m_forward = glm::vec3(m_rotation * glm::vec4(0, 0, 1, 0));

		glm::vec3 cameraRight = glm::normalize(glm::cross(m_up, m_forward));
		glm::vec3 cameraUp = glm::cross(m_forward, cameraRight);
		glm::vec3 cameraFront = glm::cross(cameraRight, cameraUp);

		m_pitch = fmax(m_pitch, -89.9f);
		m_pitch = fmin(m_pitch, 89.9f);
		m_pos += m_forward * m_velocity.z;
		m_pos += glm::normalize(glm::cross(m_forward, m_up))*m_velocity.x;
		m_pos += glm::normalize(glm::cross(m_forward, cameraRight))*m_velocity.y;

		m_view = glm::scale(glm::lookAt(m_pos, m_pos + m_forward, m_up), { -1,1,1 });
		m_velocity *= 0.90;
			

		/*float fov = std::any_cast<float>(Engine::instance()->properties()["FOV"]);
		setFov(fov);

		float znear = std::any_cast<float>(Engine::instance()->properties()["zNear"]);
		setNear(znear);

		float zfar = std::any_cast<float>(Engine::instance()->properties()["zFar"]);
		setFar(zfar);
		*/
		updateViewProjection();
	});
}

void Camera::onWindowSizeChanged(int width, int height) {
	m_bIsValid = false;
}

/**
 * Return ModelViewProjection Matrix
 */
glm::mat4x4 &Camera::viewProjection() {
	if (!m_bIsValid) {
		updateViewProjection();
	}
	return m_viewProj;
}

/**
 * Return View Matrix
 */
glm::mat4x4 &Camera::view() {
	return m_view;
}

/**
* Return Rotation Matrix
*/
glm::mat4x4 &Camera::rotation() {
	return m_rotation;
}

/**
* Return Inverse Rotation Matrix
*/
glm::mat4x4 Camera::inverseRotation() {
	return glm::inverse(m_rotation);
}

/**
 * Return Projection Matrix
 */
glm::mat4x4 &Camera::projection() {
	return m_projection;
}


void Camera::updateViewProjection() {

	omen::Engine *e = omen::Engine::instance();
	omen::Window *w = e->window();

	glm::ivec4 viewport;
	glGetIntegerv(GL_VIEWPORT, (int *)&viewport);
	float aspectRatio = (float)viewport[2] / (float)viewport[3];
	// Generates a really hard-to-read matrix, but a normal, standard 4x4 matrix nonetheless
	m_projection = glm::perspective(
		glm::radians(fov()),         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		aspectRatio, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		m_near,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		m_far       // Far clipping plane. Keep as little as possible.
		);

	// Our ModelViewProjection : multiplication of our 3 matrices
	m_viewProj = m_projection * m_view;     // Remember, matrix multiplication is the other way around
	m_bIsValid = true;
}

void Camera::setPosition(glm::vec3 position) {
	m_pos = position;
}

omen::floatprec& Camera::yaw() {
	return m_yaw;
}

omen::floatprec& Camera::pitch() {
	return m_pitch;
}

glm::vec3& Camera::acceleration() {
	return m_acceleration;
}

glm::vec3& Camera::velocity() {
	return m_velocity;
}

// The horizontal field of view in degrees
float Camera::fov() const
{
	return horizontalFieldOfView();
}

// The horizontal field of view in degrees
float Camera::horizontalFieldOfView() const
{
	return static_cast<float>(glm::degrees(2.0f*atan((0.5*m_sensorSize.x) / m_focalLength)));
}

// The vertical field of view;
float Camera::verticalFieldOfView() const
{
	return static_cast<float>(glm::degrees(2.0f*atan((0.5*m_sensorSize.y) / m_focalLength)));
}

// Set the horizontal field of view in degrees
void Camera::setFov(float fov)
{
	m_focalLength = static_cast<float>((0.5*m_sensorSize.y) / tan(glm::radians(0.5*fov)));
}

// Set the focal length
void Camera::setFocalLength(float focalLength)
{
	m_focalLength = focalLength;
}