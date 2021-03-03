#pragma once

#include <vulkan/vulkan.hpp>

namespace photonvk {
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, vk::Extent2D actualExtent);

	class AdvancedSwapchain : public vk::SwapchainKHR {
	public:
		inline AdvancedSwapchain() : vk::SwapchainKHR() {}
		inline AdvancedSwapchain(std::nullptr_t null) : vk::SwapchainKHR(null) {}
		inline AdvancedSwapchain(const vk::SwapchainKHR &swapchain) : vk::SwapchainKHR(swapchain) {}
		inline vk::Format getImageFormat() {
			return imageFormat;
		}
		inline vk::Extent2D getExtent() {
			return extent;
		}

	private:
		vk::Extent2D extent;
		vk::Format imageFormat;
		friend class AdvancedDevice;
	};
	typedef vk::UniqueHandle<AdvancedSwapchain, vk::DispatchLoaderDynamic> Swapchain;
}

template <typename Dispatch>
class vk::UniqueHandleTraits<photonvk::AdvancedSwapchain, Dispatch> {
public:
	using deleter = vk::ObjectDestroy<vk::Device, Dispatch>;
};