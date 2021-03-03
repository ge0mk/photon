#define GLFW_INCLUDE_VULKAN
#include <GLFW/window.hpp>

#include <vulkan/vulkan.h>
#include <vulkan/vertexbuffer.hpp>
#include <vulkan/indexbuffer.hpp>
#include <vulkan/uniformbuffer.hpp>
#include <vulkan/commandbuffer.hpp>
#include <vulkan/framebuffer.hpp>
#include <vulkan/descriptorset.hpp>
#include <vulkan/sync.hpp>
#include <vulkan/frame.hpp>
#include <vulkan/instance.hpp>
#include <vulkan/surface.hpp>

#include <spdlog/spdlog.h>
#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <functional>
#include <optional>
#include <set>
#include <stdexcept>

#include <cassert>
#include <cstring>
#include <cstdint>

using namespace math;

template<typename T>
std::vector<T> operator+(const std::vector<T> &a, const std::vector<T> &b) {
	std::vector<T> result(a);
	result.insert(result.end(), b.begin(), b.end());
	return result;
}

class Application {
public:
	Application(int width, int height, std::string title) : 
			extensions(Window::requiredExtensions()),
			deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME},
			validationLayers{"VK_LAYER_KHRONOS_validation"},
			win(width, height, title), 
			instance({}, extensions, validationLayers),
			surface(instance, win) {

		// init vulkan
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline();
		createCommandPool();
		createVertexBuffer();
		createIndexBuffer();
		createUniformBuffers();
		createDescriptorPool();
		createFrames();
		for(int i = 0; i < GLFW_KEY_LAST; i++)
			keys[i] = 0;
	}
	int exec() {
		win.setEventCallback([this](Event *event){this->handle(event);});
		win.setMouse(false);
		while(!win.shouldClose()) {
			win.pollEvents();
			drawFrame();
		}
		vkDeviceWaitIdle(device);
		return 0;
	}
	~Application() { // cleanup
		cleanupSwapChain();

		vkDestroyRenderPass(device, renderPass, nullptr);
		for(unsigned i = 0; i < uniformBuffers.size(); i++) {
			delete uniformBuffers[i];
		}
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);

		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

		delete indexBuffer;
		delete vertexBuffer;

		vkDestroyCommandPool(device, commandPool, nullptr);
		
		vkDestroyDevice(device, nullptr);
	}
	void handle(Event *event) {	//handle events
		dispatch<ResizeEvent>(event, [this](ResizeEvent*){
			framebufferResized = true;
		});
		dispatch<ScrollEvent>(event, [this](ScrollEvent *e){
			if(keys[GLFW_KEY_LEFT_CONTROL]) {
				fov -= e->dir.y / 20.0f;
				fov = clamp(fov, d2r(45), d2r(170));
			}
			else {
				speed += e->dir.y / 10.0f;
				speed = clamp(speed, 1.0f, 10.0f);
			}
			//rot.z += e->dir.y / 20.0f;
		});
		dispatch<MouseMoveEvent>(event, [this](MouseMoveEvent *e){
			rot.xy += math::vec2(e->dir.y, -e->dir.x) * 0.0025 * mouseacceleration;
			rot.x = math::clamp(rot.x, -math::pi/2, math::pi/2);
		});
		dispatch<KeyPressEvent>(event, [this](KeyPressEvent *e){
			keys[e->key] = true;
		});
		dispatch<KeyReleaseEvent>(event, [this](KeyReleaseEvent *e){
			keys[e->key] = false;
		});
	}
private:
	std::vector<std::string> extensions, deviceExtensions, validationLayers;
	Window win;
	vulkan::Instance instance;
	vulkan::Surface surface;

	// window management related vars
	bool framebufferResized = false;


	float fov = d2r(80);
	float speed = 1.0f;
	float mouseacceleration = 1.0f;
	vec3 rot = vec3(0);
	vec3 pos = vec3(0);

	bool keys[GLFW_KEY_LAST];

	// constants
	const int MAX_FRAMES_IN_FLIGHT = 2;

	// structs / types
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	struct UniformBufferObject {
		math::mat4 model, view, projection;
	};

	// vulkan related vars
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue, presentQueue;
	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkCommandPool commandPool;
	VkDescriptorPool descriptorPool;
	std::vector<UniformBuffer<UniformBufferObject>*> uniformBuffers;

	std::vector<vulkan::Frame> frames;
	
	VertexBuffer<vec2, vec3> *vertexBuffer;
	const std::vector<VertexBuffer<vec2, vec3>::Vertex> vertices = {
		{vec2{-0.5f, -0.5f}, vec3{1.0f, 0.0f, 0.0f}},
		{vec2{ 0.5f, -0.5f}, vec3{0.0f, 1.0f, 0.0f}},
		{vec2{ 0.5f,  0.5f}, vec3{0.0f, 0.0f, 1.0f}},
		{vec2{-0.5f,  0.5f}, vec3{1.0f, 1.0f, 1.0f}}
	};

	IndexBuffer *indexBuffer;
	const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

	// select physical device(s)
	void pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance.getHandle(), &deviceCount, nullptr);
		if(deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance.getHandle(), &deviceCount, devices.data());
		for(const VkPhysicalDevice &device : devices) {
			if(isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}
		if(physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find suitable GPU!");
		}
	}
	bool isDeviceSuitable(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		QueueFamilyIndices indices = findQueueFamilies(device);
		bool isDiscreteGpu = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
		bool extensionsSupported = checkDeviceExtensionSupport(device);
		bool swapChainAdequate = false;
		if(extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		return isDiscreteGpu && indices.isComplete() && extensionsSupported && swapChainAdequate;
	}
	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const VkExtensionProperties& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		for(unsigned i = 0; i < queueFamilies.size(); i++) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.getHandle(), &presentSupport);
			if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
			if(presentSupport) {
				indices.presentFamily = i;
			}
			if(indices.isComplete()) {
				break;
			}
		}
		return indices;
	}
	// logical devices / queue(s)
	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		float queuepriority = 1.0f;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		for(uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuepriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		std::vector<const char*> ccpextensions(deviceExtensions.size());
		std::transform(deviceExtensions.begin(), deviceExtensions.end(), ccpextensions.begin(), [](const std::string &e) { return e.c_str(); });
		std::vector<const char*> ccplayers(validationLayers.size());
		std::transform(validationLayers.begin(), validationLayers.end(), ccplayers.begin(), [](const std::string &e) { return e.c_str(); });

		VkPhysicalDeviceFeatures deviceFeatures = {};
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = uint32_t(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = uint32_t(ccpextensions.size());
		createInfo.ppEnabledExtensionNames = ccpextensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(ccplayers.size());
		createInfo.ppEnabledLayerNames = ccplayers.data();
		if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}
		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}
	// swapchain
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.getHandle(), &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getHandle(), &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getHandle(), &formatCount, details.formats.data());
		}
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getHandle(), &presentModeCount, nullptr);
		
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getHandle(), &presentModeCount, details.presentModes.data());
		}
		return details;
	}
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
		for(const VkSurfaceFormatKHR &availableFormat : availableFormats) {
			if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			//VK_PRESENT_MODE_IMMEDIATE_KHR
			if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				return availablePresentMode;
			}
		}
		if(availablePresentModes.size()) {
			return VK_PRESENT_MODE_FIFO_KHR;
		}
		else {
			throw std::runtime_error("no available swap surface formats!");
		}
	}
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(win.getHandle(), &width, &height);
			VkExtent2D actualExtent = {uint32_t(width), uint32_t(height)};

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}
	void createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = MAX_FRAMES_IN_FLIGHT;
		if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		if(swapChainSupport.capabilities.minImageCount > imageCount)
			imageCount = swapChainSupport.capabilities.minImageCount;

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface.getHandle();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if(indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}
	void cleanupSwapChain() {
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vulkan::Frame::destroy(frames, device, commandPool);
		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}
	void recreateSwapChain() {
		int width, height;
		glfwGetFramebufferSize(win.getHandle(), &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(win.getHandle(), &width, &height);
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(device);

		cleanupSwapChain();

		createSwapChain();
		createGraphicsPipeline();
		createFrames();
	}
	// render pass
	void createRenderPass() {
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}
	void createDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}
	// graphics pipeline
	void createGraphicsPipeline() {
		VkShaderModule vertShaderModule = createShaderModule(readFile("vert.spv"));
		VkShaderModule fragShaderModule = createShaderModule(readFile("frag.spv"));

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		VkVertexInputBindingDescription bindingDescription = VertexBuffer<vec2, vec3>::Vertex::getBindingDescription();
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = VertexBuffer<vec2, vec3>::Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float) swapChainExtent.width;
		viewport.height = (float) swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1; // Optional
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}
	static std::string readFile(const std::string &filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if(!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
		size_t size = file.tellg();
		std::string data(size, '\0');
		file.seekg(0);
		file.read(&data[0], size);
		file.close();
		return data;
	}
	VkShaderModule createShaderModule(const std::string &code) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;
		if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
		return shaderModule;
	}
	// command pool
	void createCommandPool() {
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
		if(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}
	void createVertexBuffer() {
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
		Buffer stagingBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, device, physicalDevice);
		void* data;
		vkMapMemory(device, stagingBuffer.bufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), bufferSize);
		vkUnmapMemory(device, stagingBuffer.bufferMemory);

		vertexBuffer = new VertexBuffer<vec2, vec3>(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device, physicalDevice);
		vertexBuffer->copyFrom(stagingBuffer, bufferSize, commandPool, graphicsQueue);
	}
	void createIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
		Buffer stagingBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, device, physicalDevice);
		void* data;
		vkMapMemory(device, stagingBuffer.bufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), bufferSize);
		vkUnmapMemory(device, stagingBuffer.bufferMemory);

		indexBuffer = new IndexBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device, physicalDevice);
		indexBuffer->copyFrom(stagingBuffer, bufferSize, commandPool, graphicsQueue);
	}
	void createUniformBuffers() {
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		uniformBuffers.resize(imageCount);
		for(unsigned i = 0; i < imageCount; i++) {
			uniformBuffers[i] = new UniformBuffer<UniformBufferObject>(device, physicalDevice);
		}
	}
	void createDescriptorPool() {
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uint32_t(imageCount) }
		};

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = sizeof(pool_sizes) / sizeof(pool_sizes[0]);
		poolInfo.pPoolSizes = &pool_sizes[0];
		poolInfo.maxSets = 1000;

		if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create desctriptor pool!");
		}
	}

	void createFrames() {
		frames = vulkan::Frame::create(device, swapChain, swapChainImageFormat, swapChainExtent, 
			renderPass, descriptorSetLayout, descriptorPool,
			[this](size_t i, VkDescriptorType &type) -> std::vector<VkDescriptorBufferInfo> {
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uniformBuffers[i]->handle;
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);

				type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				return {bufferInfo};
			}, commandPool, 
			[this](VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkDescriptorSet descriptorSet){
				VkClearValue clearColor = {0.025f, 0.03f, 0.035f, 0.0f};

				VkRenderPassBeginInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = renderPass;
				renderPassInfo.framebuffer = framebuffer;
				renderPassInfo.renderArea.offset = {0, 0};
				renderPassInfo.renderArea.extent = swapChainExtent;
				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;

				vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); // VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

				{ // move in secondary command buffer
					VkBuffer vertexBuffers[] = {vertexBuffer->getHandle()};
					VkDeviceSize offsets[] = {0};
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
					vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getHandle(), 0, VK_INDEX_TYPE_UINT16);
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, indices.size() - 3, 1, 0, 0, 0);
				} //
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
				{ // move in secondary command buffer
					VkBuffer vertexBuffers[] = {vertexBuffer->getHandle()};
					VkDeviceSize offsets[] = {0};
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
					vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getHandle(), 0, VK_INDEX_TYPE_UINT16);
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, indices.size() - 3, 1, 3, 0, 0);
				} //
				vkCmdEndRenderPass(commandBuffer);
			}
		);
	}

	void drawFrame() {
		static unsigned currentFrame = 0;
		static float t = glfwGetTime();
		float dt = glfwGetTime() - t;
		t = glfwGetTime();
		int dx = keys[GLFW_KEY_D] - keys[GLFW_KEY_A];
		int dy = keys[GLFW_KEY_LEFT_SHIFT] - keys[GLFW_KEY_SPACE];
		int dz = keys[GLFW_KEY_W] - keys[GLFW_KEY_S];
		pos += math::mat4().rotate(vec3(0, rot.y, 0)) * (math::vec4(dx, dy, dz, 0) * dt * speed);
		UniformBufferObject ubo = {
			mat4().translate(vec3(0,0,1)),
			mat4().translate(pos).rotate(rot).inverse(),
			mat4::projection(fov, float(swapChainExtent.width)/float(swapChainExtent.height), 0.0001, 10)
		};
		uniformBuffers[currentFrame]->upload(&ubo);
		if(!frames[currentFrame].draw(device, swapChain, graphicsQueue, presentQueue) || framebufferResized) {
			framebufferResized = false;
			recreateSwapChain();
		}
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
};

int main(int argc, char *argv[]) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	Application app(1080, 720, "Vulkan");
	try {
		return app.exec();
	} catch (const std::exception &e) {
		std::cerr<<e.what()<<std::endl;
		return 1;
	}
}