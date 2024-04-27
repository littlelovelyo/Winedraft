#pragma once
#include "comhdr.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <entt.hpp>

namespace glex
{
	class Transform
	{
		GLEX_STABLE_POINTER
	private:
		Transform* m_parent;
		Transform* m_child, *m_prev, *m_next;
		glm::vec3 m_position;
		glm::quat m_rotation;
		glm::vec3 m_scale;
		mutable glm::vec3 m_globalPosition;
		mutable glm::quat m_globalRotation;
		mutable glm::mat4 m_modelMat;
		mutable uint32_t m_version;
		mutable uint8_t m_dirtyFlag;
		uint8_t m_disabledComponents;
		uint16_t m_userFlag;
		entt::entity m_id;
	private:
		void SetDirtyFlag();
		void Flush() const;
	public:
		Transform(entt::entity id);
		entt::entity Entity() const { return m_id; }
		void SetParent(Transform* parent);
		void Position(glm::vec3 const& pos);
		void Rotation(glm::quat const& rot);
		void Rotation(glm::vec3 const& euler);
		void Scale(glm::vec3 const& scale);
		glm::vec3 const& Position() const { return m_position; }
		glm::quat const& Rotation() const { return m_rotation; }
		glm::vec3 const& Scale() const { return m_scale; }
		glm::vec3 const& GlobalPosition() const;
		glm::quat const& GlobalRotation() const;
		glm::mat4 const& ModelMat() const;
		glm::vec3 Forward() const;
		glm::vec3 Left() const;
		void LookTowrards(glm::vec3 const& direction);
		void Move(glm::vec3 const& pos) { Position(m_position + pos); }
		void Rotate(glm::quat const& rot) { Rotation(rot * m_rotation); }
		void Rotate(glm::vec3 const& axis, float angle);
		uint32_t Version() const { return m_version; }
		uint16_t UserFlag() const { return m_userFlag; }
		void UserFlag(uint16_t flag) { m_userFlag = flag; }

		template <typename Comp>
		void DisableComponent() {
			m_disabledComponents |= Comp::ComponentMask;
		}

		template <typename Comp>
		void EnableComponent() {
			m_disabledComponents &= ~Comp::ComponentMask;
		}

		template <typename Comp>
		bool ComponentDisabled() {
			return m_disabledComponents & Comp::ComponentMask;
		}
	};
}