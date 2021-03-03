#pragma once

#include <vulkan/vulkan.hpp>

namespace photonvk {
	typedef vk::UniqueCommandPool CommandPool;
	typedef vk::UniqueCommandBuffer CommandBuffer;

	class Buffer;
	class AdvancedBuffer : public vk::Buffer {
	public:
		inline AdvancedBuffer() : vk::Buffer(nullptr), memory(nullptr), size(0) {}
		inline AdvancedBuffer(std::nullptr_t) : vk::Buffer(nullptr), memory(nullptr), size(0) {}
		inline AdvancedBuffer(const vk::Buffer &buffer) : vk::Buffer(buffer), memory(nullptr), size(0) {}
		inline AdvancedBuffer(const AdvancedBuffer &buffer) : vk::Buffer(buffer), memory(buffer.memory), size(buffer.size) {}
		void copyFrom(const AdvancedBuffer &buffer, const CommandBuffer &commandbuffer, const vk::Queue &queue);
		inline void copyFrom(const photonvk::Buffer &buffer, const CommandBuffer &commandbuffer, const vk::Queue &queue);

		vk::DeviceMemory memory;
		vk::DeviceSize size;
	};
}

template <typename Dispatch>
class vk::UniqueHandleTraits<photonvk::AdvancedBuffer, Dispatch> {
public:
	using deleter = vk::ObjectDestroy<vk::Device, Dispatch>;
};

namespace photonvk {
	class Buffer : public vk::UniqueHandle<AdvancedBuffer, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> {
	public:
		using base = vk::UniqueHandle<AdvancedBuffer, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>;
		inline Buffer() : base() {}
		inline Buffer(const AdvancedBuffer &buffer, vk::ObjectDestroy<vk::Device, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter) : base(buffer, deleter) {}
		inline Buffer(Buffer &&other) VULKAN_HPP_NOEXCEPT : base(std::forward<Buffer>(other)) {}
		inline ~Buffer() {
			this->free();
		}
		inline void free() {
			if(*this && (*this)->memory) {
				vk::ObjectDestroy<vk::Device, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>::getOwner().free((*this)->memory);
				(*this)->memory = nullptr;
			}
		}
		inline Buffer& operator=(Buffer &&other) VULKAN_HPP_NOEXCEPT {
			this->free();
			reset(other.release());
			*static_cast<vk::ObjectDestroy<vk::Device, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>*>(this) = std::move(static_cast<vk::ObjectDestroy<vk::Device, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>&>(other));
			return *this;
		}
	};
	inline void AdvancedBuffer::copyFrom(const photonvk::Buffer &buffer, const CommandBuffer &commandbuffer, const vk::Queue &queue) {
		copyFrom(buffer.get(), commandbuffer, queue);
	}
}