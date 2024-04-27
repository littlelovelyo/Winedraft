#pragma once
#include "Component/transform.h"

namespace glex
{
	// Do we really need to make camera a component?
	class Camera
	{
	private:
		Transform const* m_transform;
		glm::mat4 m_projMat;
		mutable glm::mat4 m_viewMat;
		mutable uint32_t m_version;
	public:
		void Attach(Transform const& transform) {
			m_transform = &transform;
			m_version = 0xffffffff;
		}
		Camera() = default;
		Camera(Transform const& transform) {
			Attach(transform);
		}
		void Perspective(float fov, float aspect, float near, float far);
		glm::mat4 const& ViewMat() const;
		glm::mat4 const& ProjMat() const {
			return m_projMat;
		}
	};
}