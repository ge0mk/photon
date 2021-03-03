#pragma once

#include <vulkan/vulkan.hpp>
#include "buffer.hpp"

namespace photonvk {
	bool hasStencilComponent(vk::Format format);
	class Image;
	class AdvancedImage : public vk::Image {
	public:
		inline AdvancedImage() : vk::Image(nullptr), memory(nullptr), size(0) {}
		inline AdvancedImage(std::nullptr_t) : vk::Image(nullptr), memory(nullptr), size(0) {}
		inline AdvancedImage(const vk::Image &image) : vk::Image(image), memory(nullptr), size(0) {}
		inline AdvancedImage(const AdvancedImage &image) : vk::Image(image), memory(image.memory), size(image.size), extent(image.extent) {}
		void copyFrom(const AdvancedBuffer &buffer, const CommandBuffer &commandbuffer, const vk::Queue &queue);
		inline void copyFrom(const photonvk::Buffer &buffer, const CommandBuffer &commandbuffer, const vk::Queue &queue);
		void transitionLayout(const CommandBuffer &commandbuffer, const vk::Queue &queue, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

		vk::DeviceMemory memory;
		vk::DeviceSize size;
		vk::Extent3D extent;
	};
}

template <typename Dispatch>
class vk::UniqueHandleTraits<photonvk::AdvancedImage, Dispatch> {
public:
	using deleter = vk::ObjectDestroy<vk::Device, Dispatch>;
};

namespace photonvk {
	class Image : public vk::UniqueHandle<AdvancedImage, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> {
	public:
		using base = vk::UniqueHandle<AdvancedImage, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>;
		inline Image() : base() {}
		inline Image(const AdvancedImage &image, vk::ObjectDestroy<vk::Device, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter) : base(image, deleter) {}
		inline Image(Image &&other) VULKAN_HPP_NOEXCEPT : base(std::forward<Image>(other)) {}
		inline ~Image() {
			this->free();
		}
		inline void free() {
			if(*this && (*this)->memory) {
				vk::ObjectDestroy<vk::Device, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>::getOwner().free((*this)->memory);
				(*this)->memory = nullptr;
			}
		}
		inline Image& operator=(Image &&other) VULKAN_HPP_NOEXCEPT {
			this->free();
			reset(other.release());
			*static_cast<vk::ObjectDestroy<vk::Device, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>*>(this) = std::move(static_cast<vk::ObjectDestroy<vk::Device, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>&>(other));
			return *this;
		}
	};
	inline void AdvancedImage::copyFrom(const photonvk::Buffer &buffer, const CommandBuffer &commandbuffer, const vk::Queue &queue) {
		copyFrom(buffer.get(), commandbuffer, queue);
	}
}