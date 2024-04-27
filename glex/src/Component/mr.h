#pragma once
#include "Renderer/Gl/gl.h"
#include "Renderer/material.h"
#include <memory>

namespace glex
{
	enum class RenderOrder : uint8_t
	{
		Opaque,
		Transparent,
	};

	// Making pointer of mesh renderer stable is not a good idea
	class Renderer;
	class MeshRenderer
	{
		friend class Renderer;
	private:
		std::shared_ptr<Material> m_material;
		std::shared_ptr<GlMesh> m_mesh;
		RenderOrder m_order;
	public:
		MeshRenderer() : m_material(nullptr), m_mesh(nullptr), m_order(RenderOrder::Opaque) {}
		MeshRenderer(std::shared_ptr<Material> const& material, std::shared_ptr<GlMesh> const& mesh) :
			m_material(material), m_mesh(mesh), m_order(RenderOrder::Opaque) {}
		MeshRenderer(std::shared_ptr<Material> const& material, std::shared_ptr<GlMesh> const& mesh, RenderOrder order) :
			m_material(material), m_mesh(mesh), m_order(order) {}
		Material const* Material() const {
			return m_material.get();
		}
		GlMesh const* Mesh() const {
			return m_mesh.get();
		}
		void Mesh(std::shared_ptr<GlMesh> const& mesh) {
			m_mesh = mesh;
		}
		RenderOrder Order() const {
			return m_order;
		}
	public:
		constexpr static uint8_t ComponentMask = 1;
	};

	constexpr auto MeshRendererComparator = [](MeshRenderer const& lhs, MeshRenderer const& rhs) {
		glex::Material const* lmat = lhs.Material();
		glex::Material const* rmat = rhs.Material();
		int64_t ret;
		(ret = static_cast<int>(lhs.Order()) - static_cast<int>(rhs.Order())) || // compare order first
			(ret = rmat == nullptr ? 1 : (lmat == nullptr ? -1 : 0)) || // then check nullptr
			(ret = static_cast<int>(lmat->Shader().VertexArray().Handle()) - static_cast<int>(rmat->Shader().VertexArray().Handle())) || // compare vao next
			(ret = static_cast<int>(lmat->Shader().Handle()) - static_cast<int>(rmat->Shader().Handle())) || // then compare shader
			(ret = lmat - rmat); // compare material finally
		return ret < 0;
	};
}