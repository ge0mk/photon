#include <photonvk/mesh.hpp>

namespace photonvk {
	void Mesh::render(CommandBuffer &commandbuffer) {
		m_render(commandbuffer);
	}
	void Mesh::m_render(CommandBuffer &commandbuffer) {
		vk::Buffer vertexBuffers[] = {vertexBuffer.get()};
		vk::DeviceSize offsets[] = {0};
		commandbuffer->bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandbuffer->draw(vertices, 1, 0, 0);
	}
	void IndexedMesh::m_render(CommandBuffer &commandbuffer) {
		vk::Buffer vertexBuffers[] = {vertexBuffer.get()};
		vk::DeviceSize offsets[] = {0};
		commandbuffer->bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandbuffer->bindIndexBuffer(indexBuffer.get(), 0, vk::IndexType::eUint16);
		commandbuffer->drawIndexed(indices, 1, 0, 0, 0);
	}
}