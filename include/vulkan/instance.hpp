#pragma once

#include <vulkan/vulkan.hpp>

namespace photonvk {
	typedef vk::UniqueSurfaceKHR Surface;
	typedef vk::DispatchLoaderDynamic InstanceDispatchLoader;

	bool checkExtensionSupport(std::vector<const char*> extensions);
	bool checkValidationLayerSupport(std::vector<const char*> validationLayers);

	class PhysicalDevice;

	class AdvancedInstance : public vk::Instance {
	public:
		vk::DispatchLoaderDynamic dispatchLoader;

		inline AdvancedInstance() : vk::Instance() {}
		inline AdvancedInstance(std::nullptr_t null) : vk::Instance(null) {}
		inline AdvancedInstance(const vk::Instance &instance) : vk::Instance(instance) {}
		PhysicalDevice pickPhysicalDevice(std::function<float(PhysicalDevice)> scoreCallback);
		inline InstanceDispatchLoader createDispatchLoader() { return vk::DispatchLoaderDynamic(*this, ::vkGetInstanceProcAddr); }
	};
	typedef vk::UniqueHandle<vk::DebugUtilsMessengerEXT, InstanceDispatchLoader> DebugUtilsMessenger;
	typedef vk::UniqueHandle<AdvancedInstance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> Instance;
	Instance createInstance(const vk::ApplicationInfo &appinfo, std::vector<const char *> extensions, std::vector<const char *> validationLayers, vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateinfo = {});

}

template <typename Dispatch>
class vk::UniqueHandleTraits<photonvk::AdvancedInstance, Dispatch> {
public:
	using deleter = vk::ObjectDestroy<vk::NoParent, Dispatch>;
};