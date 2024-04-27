#pragma once
#include <stdint.h>
#include <GL/glew.h>

namespace glex
{
	class GlObj
	{
	protected:
		uint32_t m_handle = 0;
		GlObj() = default;
		GlObj(GlObj const& another) = delete;
		GlObj& operator=(GlObj const& another) = delete;
	public:
		uint32_t Handle() const {
			return m_handle;
		}
		GlObj(GlObj&& another) {
			m_handle = another.m_handle;
			another.m_handle = 0;
		}
		GlObj& operator=(GlObj&& another) {
			m_handle = another.m_handle;
			another.m_handle = 0;
			return *this;
		}
	};

	class GlBuffer : public GlObj
	{
	protected:
		uint32_t m_size = 0;
		void Create() {
			glCreateBuffers(1, &m_handle);
		}
		void Create(void const* data, uint32_t size) {
			Create();
			glNamedBufferData(m_handle, size, data, GL_STATIC_DRAW);
			m_size = size;
		}
		void Create(uint32_t size) {
			Create();
			glNamedBufferData(m_handle, size, nullptr, GL_DYNAMIC_DRAW);
			m_size = size;
		}
		void Delete() {
			glDeleteBuffers(1, &m_handle);
		}
	public:
		uint32_t Size() const {
			return m_size;
		}
		void Reload(void const* data, uint32_t size) {
			glNamedBufferData(m_handle, size, data, GL_STATIC_DRAW);
			m_size = size;
		}
		void Update(uint32_t offset, void const* data, uint32_t size) {
			glNamedBufferSubData(m_handle, offset, size, data);
		}
		template <typename T> T* Map() {
			return reinterpret_cast<T*>(glMapNamedBuffer(m_handle, GL_WRITE_ONLY));
		}
		bool Unmap() {
			return glUnmapNamedBuffer(m_handle);
		}
	};
}