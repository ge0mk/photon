#include <photonvk/instance.hpp>
#include <iostream>

#include <photonvk/physicaldevice.hpp>
#include <spdlog/spdlog.h>

namespace photonvk {
	bool checkExtensionSupport(std::vector<const char*> extensions) {
		bool allSupported = true;
		std::vector<vk::ExtensionProperties> available = vk::enumerateInstanceExtensionProperties();
		for(const char *extension : extensions) {
			if( std::find_if(available.begin(), available.end(), [&extension](const vk::ExtensionProperties &e) {
					return strcmp(extension, e.extensionName) == 0;
				}) == available.end()) {
				spdlog::error("photonvk: extension \"{}\" is not supported!", extension);
				allSupported = false;
			}
		}
		return allSupported;
	}

	bool checkValidationLayerSupport(std::vector<const char*> validationLayers) {
		bool allSupported = true;
		std::vector<vk::LayerProperties> available = vk::enumerateInstanceLayerProperties();
		for(const char *layer : validationLayers) {
			if( std::find_if(available.begin(), available.end(), [&layer](const vk::LayerProperties &e) {
					return strcmp(layer, e.layerName) == 0;
				}) == available.end()) {
				spdlog::error("photonvk: validation layer \"{}\" is not supported!", layer);
				allSupported = false;
			}
		}
		return allSupported;
	}
	
	PhysicalDevice AdvancedInstance::pickPhysicalDevice(std::function<float(PhysicalDevice)> scoreCallback) {
		std::vector<vk::PhysicalDevice> devices = enumeratePhysicalDevices();
		float topscore = 0.0f;
		int index = -1;
		for(unsigned i = 0; i < devices.size(); i++) {
			float score = scoreCallback(devices[i]);
			if(score > topscore) {
				topscore = score;
				index = i;
			}
		}
		if(index >= 0) {
			return devices[index];
		}
		return nullptr;
	}

	Instance createInstance(const vk::ApplicationInfo &appinfo, std::vector<const char *> extensions, std::vector<const char *> validationLayers, vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateinfo) {
		if(validationLayers.size()) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		if(!checkExtensionSupport(extensions)) {
			throw std::runtime_error("not all required extensions are supported!");
		}
		if(!checkValidationLayerSupport(validationLayers)) {
			throw std::runtime_error("not all validation layers are available!");
		}
		for(const char *e : extensions) {
			spdlog::info("need extension \"{}\"", e);
		}
		vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> createinfo(
			{
				{},
				&appinfo,
				uint32_t(validationLayers.size()),
				validationLayers.data(),
				uint32_t(extensions.size()),
				extensions.data()
			}, debugMessengerCreateinfo
		);
		AdvancedInstance instance = vk::createInstance(createinfo.get<vk::InstanceCreateInfo>(), nullptr);
		instance.dispatchLoader = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
		return Instance(instance, vk::ObjectDestroy<vk::NoParent, vk::DispatchLoaderStatic>(nullptr));
	}
}