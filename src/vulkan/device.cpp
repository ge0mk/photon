#include <photonvk/device.hpp>
#include <photonvk/physicaldevice.hpp>
#include <photonvk/swapchain.hpp>

#include <fstream>

namespace photonvk {
	RenderPass AdvancedDevice::createRenderPass(vk::Format swapchainImageFormat, vk::Format depthBufferFormat, vk::ImageLayout initialLayout) {
		vk::AttachmentDescription colorAttachment(
			{}, swapchainImageFormat, vk::SampleCountFlagBits::e1,
			initialLayout == vk::ImageLayout::eUndefined ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
			initialLayout, vk::ImageLayout::ePresentSrcKHR
		);
		vk::AttachmentDescription depthAttachment(
			{}, depthBufferFormat, vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal
		);
		vk::AttachmentReference colorAttachmentRef(
			0, vk::ImageLayout::eColorAttachmentOptimal
		);
		vk::AttachmentReference depthAttachmentRef(
			1, vk::ImageLayout::eDepthStencilAttachmentOptimal
		);
		vk::SubpassDescription subpass(
			{}, vk::PipelineBindPoint::eGraphics,
			0, nullptr,
			1, &colorAttachmentRef,
			nullptr, &depthAttachmentRef,
			0, nullptr
		);
		vk::SubpassDependency dependency(
			VK_SUBPASS_EXTERNAL, 0,
			vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::AccessFlagBits(0), vk::AccessFlagBits::eColorAttachmentWrite
		);
		std::vector<vk::AttachmentDescription> attachments = {colorAttachment, depthAttachment};
		vk::RenderPassCreateInfo createinfo(
			{},
			attachments.size(), attachments.data(),
			1, &subpass,
			1, &dependency
		);
		return createRenderPassUnique(createinfo);
	}
	DescriptorSetLayout AdvancedDevice::createDescriptorSetLayout(std::vector<vk::DescriptorSetLayoutBinding> bindings) {
		vk::DescriptorSetLayoutCreateInfo createinfo(
			{}, bindings.size(), bindings.data()
		);
		return createDescriptorSetLayoutUnique(createinfo);
	}
	ShaderModule AdvancedDevice::createShaderModule(const std::string &code) {
		vk::ShaderModuleCreateInfo createinfo(
			{}, code.size(), reinterpret_cast<const uint32_t*>(code.data())
		);
		return createShaderModuleUnique(createinfo);
	}
	ShaderModule AdvancedDevice::loadShaderModule(const std::string &filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if(file.is_open()) {
			size_t size = file.tellg();
			std::string code(size, 0);
			file.seekg(0);
			file.read(&code[0], size);
			file.close();
			return createShaderModule(code);
		}
		return createShaderModule("");
	}
	Swapchain AdvancedDevice::createSwapchain(PhysicalDevice &physicaldevice, const Surface &surface, vk::Extent2D actualExtent, uint32_t imagecount) {
		Swapchain tmp(nullptr, vk::ObjectDestroy<vk::Device, vk::DispatchLoaderDynamic>(*this, nullptr, dispatchLoader));
		return recreateSwapchain(tmp, physicaldevice, surface, actualExtent, imagecount);
	}
	Swapchain AdvancedDevice::recreateSwapchain(const Swapchain &oldSwapchain, PhysicalDevice &physicaldevice, const Surface &surface, vk::Extent2D actualExtent, uint32_t imagecount) {
		auto indices = physicaldevice.findQueueFamilies(surface);
		auto swapChainSupport = physicaldevice.querySwapChainSupport(surface);

		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities, actualExtent);

		if(swapChainSupport.capabilities.maxImageCount > 0) {
			if(imagecount > swapChainSupport.capabilities.maxImageCount) {
				imagecount = swapChainSupport.capabilities.maxImageCount;
			}
		}
		if(imagecount < swapChainSupport.capabilities.minImageCount) {
			imagecount = swapChainSupport.capabilities.minImageCount;
		}

		vk::SwapchainCreateInfoKHR createinfo(
			{}, surface.get(), imagecount,
			surfaceFormat.format, surfaceFormat.colorSpace,
			extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
			(indices.graphicsFamily != indices.presentFamily ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive),
			(indices.graphicsFamily != indices.presentFamily ? 2 : 0),
			(indices.graphicsFamily != indices.presentFamily ? queueFamilyIndices : nullptr),
			swapChainSupport.capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
			presentMode, VK_TRUE, oldSwapchain.get()
		);
		AdvancedSwapchain swapchain = createSwapchainKHR(createinfo, nullptr, dispatchLoader);
		if(swapchain) {
			swapchain.extent = extent;
			swapchain.imageFormat = surfaceFormat.format;
			return vk::UniqueHandle<AdvancedSwapchain, vk::DispatchLoaderDynamic>(swapchain, vk::ObjectDestroy<vk::Device, vk::DispatchLoaderDynamic>(*this, nullptr, dispatchLoader));
		}
		else {
			throw std::runtime_error("failed to create swapchain!");
		}
		return vk::UniqueHandle<AdvancedSwapchain, vk::DispatchLoaderDynamic>(nullptr, vk::ObjectDestroy<vk::Device, vk::DispatchLoaderDynamic>(*this, nullptr, dispatchLoader));
	}
	Buffer AdvancedDevice::createBuffer(const PhysicalDevice &physicaldevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::MemoryPropertyFlags properties) {
		vk::BufferCreateInfo createinfo(
			{}, size, usage, sharingMode
		);
		AdvancedBuffer buffer = vk::Device::createBuffer(createinfo);
		vk::MemoryRequirements memRequirements = getBufferMemoryRequirements(buffer);
		vk::MemoryAllocateInfo allocinfo(
			memRequirements.size, physicaldevice.findMemoryType(memRequirements.memoryTypeBits, properties)
		);
		buffer.memory = allocateMemory(allocinfo);
		buffer.size = size;
		bindBufferMemory(buffer, buffer.memory, 0);
		return Buffer(buffer, vk::ObjectDestroy<vk::Device, vk::DispatchLoaderStatic>(*this));
	}
	Image AdvancedDevice::createImage(const PhysicalDevice &physicaldevice, vk::Extent3D extent, vk::Format format, vk::ImageTiling tiling, vk::ImageType type, vk::ImageUsageFlags usage, vk::SharingMode sharingMode, vk::MemoryPropertyFlags properties) {
		vk::DeviceSize size = extent.width * extent.height * extent.depth * 4;
		vk::ImageCreateInfo createinfo({},
			type, format, extent,
			1, 1, vk::SampleCountFlagBits::e1,
			tiling, usage, sharingMode,
			0, nullptr,
			vk::ImageLayout::eUndefined
		);
		AdvancedImage image = vk::Device::createImage(createinfo);
		vk::MemoryRequirements memRequirements = getImageMemoryRequirements(image);
		vk::MemoryAllocateInfo allocinfo(
			memRequirements.size, physicaldevice.findMemoryType(memRequirements.memoryTypeBits, properties)
		);
		image.memory = allocateMemory(allocinfo);
		image.size = size;
		image.extent = extent;
		bindImageMemory(image, image.memory, 0);
		return Image(image, vk::ObjectDestroy<vk::Device, vk::DispatchLoaderStatic>(*this));
	}
	PipelineLayout AdvancedDevice::createPipelineLayout(std::vector<vk::DescriptorSetLayout> descriptorSetLayouts, std::vector<vk::PushConstantRange> pushConstantRanges) {
		vk::PipelineLayoutCreateInfo createinfo(
			{}, 
			descriptorSetLayouts.size(), descriptorSetLayouts.data(), 
			pushConstantRanges.size(), pushConstantRanges.data()
		);
		vk::PipelineLayout layout = vk::Device::createPipelineLayout(createinfo);
		return PipelineLayout(layout, vk::ObjectDestroy<vk::Device, vk::DispatchLoaderStatic>(*this));
	}
	CommandPool AdvancedDevice::createCommandPool(uint32_t queueFamily, vk::CommandPoolCreateFlags flags) {
		vk::CommandPoolCreateInfo createinfo(flags, queueFamily);
		vk::CommandPool pool = vk::Device::createCommandPool(createinfo);
		return CommandPool(pool, vk::ObjectDestroy<vk::Device, vk::DispatchLoaderStatic>(*this));
	}
	DescriptorPool AdvancedDevice::createDescriptorPool(std::vector<vk::DescriptorPoolSize> poolsizes, uint32_t maxSets, vk::DescriptorPoolCreateFlags flags) {
		vk::DescriptorPoolCreateInfo createinfo(flags,
			maxSets, poolsizes.size(), poolsizes.data()
		);
		vk::DescriptorPool pool = vk::Device::createDescriptorPool(createinfo);
		return DescriptorPool(pool, vk::ObjectDestroy<vk::Device, vk::DispatchLoaderStatic>(*this));
	}
	uint32_t AdvancedDevice::getSwapchainImageCount(const Swapchain &swapchain) {
		auto images = getSwapchainImagesKHR(swapchain.get(), dispatchLoader);
		return images.size();
	}
	std::vector<vk::Image> AdvancedDevice::getSwapchainImages(const Swapchain &swapchain) {
		std::vector<vk::Image> images = getSwapchainImagesKHR(swapchain.get(), dispatchLoader);
		return images;
	}
	void AdvancedDevice::setBufferData(const Buffer &buffer, const void *data, size_t size) {
		void *mem = mapMemory(buffer->memory, 0, size, {});
		memcpy(mem, data, size);
		unmapMemory(buffer->memory);
	}
	CommandBuffer AdvancedDevice::allocateCommandBuffer(const CommandPool &pool, vk::CommandBufferLevel level) {
		vk::CommandBufferAllocateInfo allocinfo(
			pool.get(), level, 1
		);
		vk::CommandBuffer buffer;
		(void)vk::Device::allocateCommandBuffers(&allocinfo, &buffer);

		vk::PoolFree<vk::Device, vk::CommandPool, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter(*this, pool.get());
		return CommandBuffer(buffer, deleter);
	}
	std::vector<CommandBuffer> AdvancedDevice::allocateCommandBuffers(const CommandPool &pool, size_t count, vk::CommandBufferLevel level) {
		std::vector<CommandBuffer> commandBuffers;
		vk::CommandBufferAllocateInfo allocinfo(
			pool.get(), level, count
		);
		std::vector<vk::CommandBuffer> buffer = vk::Device::allocateCommandBuffers(allocinfo);

		vk::PoolFree<vk::Device, vk::CommandPool, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter(*this, pool.get());
		for(size_t i = 0; i < count; i++)	{
			commandBuffers.push_back(CommandBuffer(buffer[i], deleter));
		}
		return commandBuffers;
	}
	ImageView AdvancedDevice::createImageView(const vk::Image &image, vk::Format imageFormat, vk::ImageAspectFlags aspectFlags) {
		vk::ImageViewCreateInfo createinfo({},
			image, vk::ImageViewType::e2D, imageFormat,
			vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity),
			vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1)
		);
		return createImageViewUnique(createinfo);
	}
	std::vector<ImageView> AdvancedDevice::createImageViews(const std::vector<vk::Image> &images, vk::Format imageFormat, vk::ImageAspectFlags aspectFlags) {
		std::vector<ImageView> imageViews;
		for(const vk::Image &image : images) {
			imageViews.push_back(createImageView(image, imageFormat, aspectFlags));
		}
		return imageViews;
	}
	Framebuffer AdvancedDevice::createFramebuffer(const std::vector<vk::ImageView> &attachments, vk::Extent2D extend, const RenderPass &renderpass, uint32_t layers, vk::FramebufferCreateFlags flags) {
		vk::FramebufferCreateInfo createinfo(flags,
			renderpass.get(),
			attachments.size(), attachments.data(),
			extend.width, extend.height,
			layers
		);
		return createFramebufferUnique(createinfo);
	}
	std::vector<Framebuffer> AdvancedDevice::createFramebuffers(const std::vector<std::vector<vk::ImageView>> &attachments, vk::Extent2D extend, const RenderPass &renderpass, uint32_t layers, vk::FramebufferCreateFlags flags) {
		std::vector<Framebuffer> framebuffers;
		for(auto a : attachments) {
			framebuffers.push_back(createFramebuffer(a, extend, renderpass, layers, flags));
		}
		return framebuffers;
	}
	std::vector<DescriptorSet> AdvancedDevice::createDescriptorSets(const DescriptorPool &pool, const std::vector<vk::DescriptorSetLayout> &layouts) {
		vk::DescriptorSetAllocateInfo allocinfo(pool.get(), layouts.size(), layouts.data());
		return allocateDescriptorSetsUnique(allocinfo);
	}
	Semaphore AdvancedDevice::createSemaphore() {
		vk::SemaphoreCreateInfo createinfo;
		return createSemaphoreUnique(createinfo);
	}
	std::vector<Semaphore> AdvancedDevice::createSemaphores(size_t count) {
		std::vector<Semaphore> semaphores(count);
		for(Semaphore &semaphore : semaphores) {
			semaphore = createSemaphore();
		}
		return semaphores;
	}
	Fence AdvancedDevice::createFence(vk::FenceCreateFlags flags) {
		vk::FenceCreateInfo createinfo(flags);
		return createFenceUnique(createinfo);
	}
	std::vector<Fence> AdvancedDevice::createFences(size_t count, vk::FenceCreateFlags flags) {
		std::vector<Fence> fences(count);
		for(Fence &fence : fences) {
			fence = createFence(flags);
		}
		return fences;
	}
	Sampler AdvancedDevice::createSampler(vk::Filter filter, vk::SamplerAddressMode addressmode, float maxAnisotropy, vk::BorderColor bordercolor) {
		vk::SamplerCreateInfo createinfo({},
			filter, filter, vk::SamplerMipmapMode::eLinear,
			addressmode, addressmode, addressmode,
			0.0f, maxAnisotropy > 0.0f, maxAnisotropy,
			false, vk::CompareOp::eAlways,
			0.0f, 0.0f, bordercolor, false
		);
		return createSamplerUnique(createinfo);
	}
}