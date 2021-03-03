#include <vulkan/physicaldevice.hpp>

#include <set>

namespace photonvk {
	bool PhysicalDevice::checkExtensionSupport(std::vector<const char*> extensions, AdvancedInstance instance) const {
		std::vector<vk::ExtensionProperties> available = enumerateDeviceExtensionProperties();
		for(const char *extension : extensions) {
			if( std::find_if(available.begin(), available.end(), [&extension](const vk::ExtensionProperties &e) {
					return strcmp(extension, e.extensionName) == 0;
				}) == available.end()) {
				return false;
			}
		}
		return true;
	}

	QueueFamilyIndices PhysicalDevice::findQueueFamilies(const Surface &surface, AdvancedInstance instance) const {
		std::vector<vk::QueueFamilyProperties> queueFamilies = instance ? getQueueFamilyProperties(instance.dispatchLoader) : getQueueFamilyProperties();
		QueueFamilyIndices indices;
		for(unsigned i = 0; i < queueFamilies.size(); i++) {
			if(queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
			}
			if(getSurfaceSupportKHR(i, surface.get())) {
				indices.presentFamily = i;
			}
			if(indices) {
				break;
			}
		}
		return indices;
	}

	SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(const Surface &surface, AdvancedInstance instance) const {
		SwapChainSupportDetails swapChainSupportDetails;
		if(instance) {
			swapChainSupportDetails.capabilities = getSurfaceCapabilitiesKHR(surface.get(), instance.dispatchLoader);
			swapChainSupportDetails.formats = getSurfaceFormatsKHR(surface.get(), instance.dispatchLoader);
			swapChainSupportDetails.presentModes = getSurfacePresentModesKHR(surface.get(), instance.dispatchLoader);
		}
		else {
			swapChainSupportDetails.capabilities = getSurfaceCapabilitiesKHR(surface.get());
			swapChainSupportDetails.formats = getSurfaceFormatsKHR(surface.get());
			swapChainSupportDetails.presentModes = getSurfacePresentModesKHR(surface.get());
		}
		return swapChainSupportDetails;
	}

	Device PhysicalDevice::createDevice(const Surface &surface, std::vector<const char*> deviceExtensions, std::vector<const char*> validationLayers, AdvancedInstance instance) {
		QueueFamilyIndices indices = findQueueFamilies(surface);
		float queuePriority = 1.0f;

		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

		for(uint32_t queueFamily : uniqueQueueFamilies) {
			queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(
				{}, queueFamily, 1, &queuePriority
			));
		}

		vk::PhysicalDeviceFeatures deviceFeatures;
		vk::DeviceCreateInfo createInfo(
			{}, 
			queueCreateInfos.size(), queueCreateInfos.data(), 
			validationLayers.size(), validationLayers.data(),
			deviceExtensions.size(), deviceExtensions.data(),
			&deviceFeatures
		);
		AdvancedDevice dev = base::createDevice(createInfo);
		dev.setQueueFamilyIndices(indices);
		dev.dispatchLoader = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr, dev, vkGetDeviceProcAddr);
		return Device(dev, vk::ObjectDestroy<vk::NoParent, vk::DispatchLoaderStatic>(nullptr));
	}
	
	uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const {
		vk::PhysicalDeviceMemoryProperties memProperties = getMemoryProperties();
		for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if(typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}

	vk::Format PhysicalDevice::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const {
		for (vk::Format format : candidates) {
			vk::FormatProperties props = getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		throw std::runtime_error("no suitable format found!");
		return vk::Format::eUndefined;
	}

	vk::Format PhysicalDevice::findDepthBufferFormat() const {
		return findSupportedFormat(
			{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);
	}
}