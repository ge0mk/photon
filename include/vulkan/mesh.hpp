#pragma once

#include <vulkan/vulkan.hpp>
#include <photonvk/buffer.hpp>
#include <math/matrix.hpp>

namespace photonvk {
	class Mesh {
	public:
		inline Mesh() : vertexBuffer(), vertices(0), transform(math::mat4()) {}
		inline Mesh(Buffer &&vertexBuffer, uint32_t vertices = 0)
			 : vertexBuffer(std::forward<Buffer>(vertexBuffer)), vertices(vertices), transform(math::mat4()) {}
		virtual inline ~Mesh() {}
		void render(CommandBuffer &commandbuffer);
		virtual void m_render(CommandBuffer &commandbuffer);

	protected:
		Buffer vertexBuffer;
		uint32_t vertices;

	public:
		math::mat4 transform;
	};

	class IndexedMesh : public Mesh {
	public:
		inline IndexedMesh(Buffer &&vertexBuffer, Buffer &&indexBuffer, uint32_t indices = 0)
			 : Mesh(std::forward<Buffer>(vertexBuffer)), indexBuffer(std::forward<Buffer>(indexBuffer)), indices(indices) {}
		inline ~IndexedMesh() {}
		void m_render(CommandBuffer &commandbuffer);

	protected:
		Buffer indexBuffer;
		uint32_t indices;
	};
}