#pragma once
#include <entt.hpp>
#include "Component/transform.h"
#include "Component/mr.h"
#include "camera.h"

namespace glex
{
	using Entity = entt::entity;
	using InsertionSort = entt::insertion_sort;
	using IntroSort = entt::std_sort;
	class Renderer;

	class Scene
	{
		friend class Renderer;
	private:
		entt::registry m_registry;
		bool m_reorderMeshRenderers;
	private:
		void ReorderMeshRenderers(entt::registry& registry, entt::entity enity) {
			m_reorderMeshRenderers = true;
		}
	public:
		Scene() {
			m_registry.on_construct<MeshRenderer>().connect<&Scene::ReorderMeshRenderers>(*this);
			m_registry.on_destroy<MeshRenderer>().connect<&Scene::ReorderMeshRenderers>(*this);
			m_registry.on_update<MeshRenderer>().connect<&Scene::ReorderMeshRenderers>(*this);
		}
		void Clear() {
			m_registry.clear();
		}
		Transform& AddEntity() {
			Entity entity = m_registry.create();
			Transform& transform = m_registry.emplace<Transform>(entity, entity);
			return transform;
		}
		void DestroyEntity(Entity entity) {
			m_registry.destroy(entity);
		}
		void DestroyEntity(Transform& transform) {
			m_registry.destroy(transform.Entity());
		}
		template <typename Comp, typename... Args>
		Comp& AddComponent(Entity entity, Args&&... args) {
			return m_registry.emplace<Comp>(entity, std::forward<Args>(args)...);
		}
		template <typename Comp, typename... Args>
		Comp& AddComponent(Transform const& transform, Args&&... args) {
			return m_registry.emplace<Comp>(transform.Entity(), std::forward<Args>(args)...);
		}
		template <typename Comp>
		void RemoveComponent(Entity entity) {
			m_registry.remove<Comp>(entity);
		}
		template <typename Comp>
		void RemoveComponent(Transform const& transform) {
			m_registry.remove<Comp>(transform.Entity());
		}
		template <typename Comp>
		Comp& GetComponent(Entity entity) {
			return m_registry.get<Comp>(entity);
		}
		template <typename Comp>
		Comp& GetComponent(Transform const& transform) {
			return m_registry.get<Comp>(transform.Entity());
		}
		template <typename Component, typename Comparator, typename Algorithm>
			requires std::is_same_v<Component, MeshRenderer>
		void Sort(Comparator comp, Algorithm algo = InsertionSort()) {
			m_registry.sort<MeshRenderer>(std::move(comp), std::move(algo));
			m_reorderMeshRenderers = false;
		}
		template <typename Component, typename Comparator, typename Algorithm>
			requires (!std::is_same_v<Component, MeshRenderer>)
		void Sort(Comparator comp, Algorithm algo = InsertionSort()) {
			m_registry.sort<Component>(std::move(comp), std::move(algo));
		}
	};
}