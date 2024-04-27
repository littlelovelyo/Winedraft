#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glex;

void Camera::Perspective(float fov, float aspect, float near, float far)
{
	m_projMat = glm::perspective(fov, aspect, near, far);
}

glm::mat4 const& Camera::ViewMat() const
{
	if (m_version != m_transform->Version())
	{
		glm::vec3 pos = m_transform->GlobalPosition();
		glm::vec3 target = pos + m_transform->Forward();
		m_viewMat = glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
		m_version = m_transform->Version();
	}
	return m_viewMat;
}