#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glex;

Transform::Transform(entt::entity id) :
	m_id(id), m_parent(nullptr), m_child(nullptr), m_prev(nullptr), m_next(nullptr),
	m_position(0.0f, 0.0f, 0.0f), m_scale(1.0f, 1.0f, 1.0f), m_rotation(1.0f, 0.0f, 0.0f, 0.0f),
	m_version(0), m_dirtyFlag(3), m_disabledComponents(0), m_userFlag(0) {}

void Transform::SetDirtyFlag()
{
	m_version++;
	m_dirtyFlag = 3;
	for (Transform* p = m_child; p != nullptr; p = p->m_next)
		p->SetDirtyFlag();
}

void Transform::Flush() const
{
	if (m_dirtyFlag & 1)
	{
		if (m_parent != nullptr)
		{
			m_globalPosition = m_parent->GlobalPosition() + m_position;
			m_globalRotation = m_rotation * m_parent->GlobalRotation();
		}
		else
		{
			m_globalPosition = m_position;
			m_globalRotation = m_rotation;
		}
		m_dirtyFlag &= ~1;
	}
}

void Transform::SetParent(Transform* parent)
{
	if (m_parent != nullptr)
	{
		if (m_parent->m_child == this)
			parent->m_child = m_next;
		if (m_prev != nullptr)
			m_prev->m_next = m_next;
		if (m_next != nullptr)
			m_next->m_prev = m_prev;
		m_prev = nullptr;
		m_next = nullptr;
	}
	m_parent = parent;
	if (parent != nullptr)
	{
		Transform* head = parent->m_child;
		if (head != nullptr)
			head->m_prev = this;
		m_next = head;
		parent->m_child = this;
	}
	SetDirtyFlag();
}

glm::vec3 const& Transform::GlobalPosition() const
{
	Flush();
	return m_globalPosition;
}

glm::quat const& Transform::GlobalRotation() const
{
	Flush();
	return m_globalRotation;
}

glm::mat4 const& Transform::ModelMat() const
{
	if (m_dirtyFlag & 2)
	{
		Flush();
		m_modelMat = glm::mat4(1.0f);
		m_modelMat = glm::scale(m_modelMat, m_scale);
		m_modelMat = glm::mat4_cast(m_globalRotation) * m_modelMat;
		// glm applies scale to translation for some reason
		m_modelMat[3] = glm::vec4(m_globalPosition, 1.0f);
		m_dirtyFlag = 0;
	}
	return m_modelMat;
}

glm::vec3 Transform::Forward() const
{
	return glm::rotate(m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
}

glm::vec3 Transform::Left() const
{
	return glm::rotate(m_rotation, glm::vec3(1.0f, 0.0f, 0.0f));
}

void Transform::Position(glm::vec3 const& pos)
{
	m_position = pos;
	SetDirtyFlag();
}

void Transform::Rotation(glm::quat const& rot)
{
	m_rotation = rot;
	SetDirtyFlag();
}

void Transform::Rotation(glm::vec3 const& euler)
{
	m_rotation = glm::quat(euler);
	SetDirtyFlag();
}

void Transform::Scale(glm::vec3 const& scale)
{
	// scale doesn't have a hierarchical influence
	m_scale = scale;
	m_dirtyFlag = 3;
}

void Transform::LookTowrards(glm::vec3 const& direction)
{
	/* constexpr float epsilon = glm::epsilon<float>();
	glm::vec3 dir = glm::normalize(direction);
	glm::vec3 i = glm::vec3(0.0f, 0.0f, 1.0f);
	float dot = glm::dot(dir, i);
	// look front
	if (glm::abs(1.0f - dot) < epsilon)
		m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	// look back
	else if (glm::abs(dot + 1.0f) < epsilon)
		m_rotation = glm::quat(0.0f, 0.0f, 1.0f, 0.0f);
	// other situation
	else
	{
		glm::vec3 axis = glm::normalize(glm::cross(i, dir));
		float cos = glm::dot(dir, i);
		float theta = glm::acos(cos) / 2.0f;
		m_rotation = glm::quat(glm::cos(theta), glm::sin(theta) * axis);
	} */
	m_rotation = glm::quat(glm::vec3(0.0f, 0.0f, 1.0f), glm::normalize(direction));
	SetDirtyFlag();
}

void Transform::Rotate(glm::vec3 const& axis, float angle)
{
	m_rotation = glm::rotate(m_rotation, angle, axis);
	SetDirtyFlag();
}