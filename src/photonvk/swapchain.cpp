#include <photonvk/swapchain.hpp>

#include <math/math.hpp>
#include <vector>

namespace photonvk {
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
		for(const vk::SurfaceFormatKHR &availableFormat : availableFormats) {
			if(availableFormat.format == vk::Format::eR8G8B8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}
		if(availableFormats.size()) {
			return availableFormats[0];
		}
		else {
			throw std::runtime_error("no available swap surface formats!");
		}
	}
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
		for (const auto &mode : availablePresentModes) {
			//vk::PresentModeKHR::eMailbox
			if (mode == vk::PresentModeKHR::eImmediate) {
				return mode;
			}
		}
		if(availablePresentModes.size()) {
			return vk::PresentModeKHR::eFifo;
		}
		else {
			throw std::runtime_error("no available swap surface formats!");
		}
	}
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, vk::Extent2D actualExtent) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			actualExtent.width = math::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = math::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}
}