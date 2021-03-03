#pragma once

#include <vulkan/vulkan.hpp>
#include "instance.hpp"
#include "device.hpp"

namespace photonvk {
	class PhysicalDevice : public vk::PhysicalDevice {
		using base = vk::PhysicalDevice;
	public:
		template<typename ...Args>
		PhysicalDevice(Args ...args) : base(args...) {}

		bool checkExtensionSupport(std::vector<const char*> extensions, AdvancedInstance instance = nullptr) const;
		QueueFamilyIndices findQueueFamilies(const Surface &surface, AdvancedInstance instance = nullptr) const;
		SwapChainSupportDetails querySwapChainSupport(const Surface &surface, AdvancedInstance instance = nullptr) const;
		Device createDevice(const Surface &surface, std::vector<const char*> deviceExtensions, std::vector<const char*> validationLayers = {}, AdvancedInstance instance = nullptr);
		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
		vk::Format findSupportedFormat(const std::vector<vk::Format> &formats, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
		vk::Format findDepthBufferFormat() const;
	};
}