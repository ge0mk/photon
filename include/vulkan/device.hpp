#pragma once

#include <vulkan/vulkan.hpp>
#include <photonvk/instance.hpp>
#include <photonvk/buffer.hpp>
#include <photonvk/image.hpp>
#include <photonvk/vertex.hpp>
#include <photonvk/swapchain.hpp>

#include <optional>

namespace photonvk {
	class PhysicalDevice;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		inline operator bool() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	typedef vk::UniqueRenderPass RenderPass;
	typedef vk::UniqueShaderModule ShaderModule;
	typedef vk::UniquePipelineLayout PipelineLayout;
	typedef vk::UniquePipeline Pipeline;
	typedef vk::UniqueDescriptorSetLayout DescriptorSetLayout;
	typedef vk::UniqueDescriptorPool DescriptorPool;
	typedef vk::UniqueDescriptorSet DescriptorSet;
	typedef vk::UniqueImageView ImageView;
	typedef vk::UniqueFramebuffer Framebuffer;
	typedef vk::UniqueSemaphore Semaphore;
	typedef vk::UniqueFence Fence;
	typedef vk::UniqueSampler Sampler;

	class AdvancedDevice : public vk::Device {
	public:
		inline AdvancedDevice() : vk::Device(), indices() {}
		inline AdvancedDevice(std::nullptr_t null) : vk::Device(null), indices() {}
		inline AdvancedDevice(const vk::Device &device) : vk::Device(device), indices() {}

		vk::DispatchLoaderDynamic dispatchLoader;

		RenderPass createRenderPass(vk::Format swapchainImageFormat, vk::Format depthBufferFormat = {}, vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined);
		DescriptorSetLayout createDescriptorSetLayout(std::vector<vk::DescriptorSetLayoutBinding> bindings);
		ShaderModule createShaderModule(const std::string &code);
		ShaderModule loadShaderModule(const std::string &filename);
		Swapchain createSwapchain(PhysicalDevice &physicaldevice, const Surface &surface, vk::Extent2D actualExtent, uint32_t imagecount = 2);
		Swapchain recreateSwapchain(const Swapchain &oldSwapchain, PhysicalDevice &physicaldevice, const Surface &surface, vk::Extent2D actualExtent, uint32_t imagecount = 2);
		Buffer createBuffer(const PhysicalDevice &physicaldevice, vk::DeviceSize size, vk::BufferUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {});
		Image createImage(const PhysicalDevice &physicaldevice, vk::Extent3D extent, vk::Format format, vk::ImageTiling tiling, vk::ImageType type, vk::ImageUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {});
		PipelineLayout createPipelineLayout(std::vector<vk::DescriptorSetLayout> descriptorSetLayouts, std::vector<vk::PushConstantRange> pushConstantRanges = {});
		CommandPool createCommandPool(uint32_t queueFamily, vk::CommandPoolCreateFlags flags = {});
		DescriptorPool createDescriptorPool(std::vector<vk::DescriptorPoolSize> poolsizes, uint32_t maxSets = 1000, vk::DescriptorPoolCreateFlags flags = {});
		uint32_t getSwapchainImageCount(const Swapchain &swapchain);
		std::vector<vk::Image> getSwapchainImages(const Swapchain &swapchain);
		void setBufferData(const Buffer &buffer, const void *data, size_t size);
		CommandBuffer allocateCommandBuffer(const CommandPool &pool, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
		std::vector<CommandBuffer> allocateCommandBuffers(const CommandPool &pool, size_t count, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
		ImageView createImageView(const vk::Image &image, vk::Format swapchainImageFormat, vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);
		std::vector<ImageView> createImageViews(const std::vector<vk::Image> &image, vk::Format imageFormat, vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);
		Framebuffer createFramebuffer(const std::vector<vk::ImageView> &attachments, vk::Extent2D extend, const RenderPass &renderpass, uint32_t layers = 1, vk::FramebufferCreateFlags = {});
		std::vector<Framebuffer> createFramebuffers(const std::vector<std::vector<vk::ImageView>> &attachments, vk::Extent2D extend, const RenderPass &renderpass, uint32_t layers = 1, vk::FramebufferCreateFlags = {});
		std::vector<DescriptorSet> createDescriptorSets(const DescriptorPool &pool, const std::vector<vk::DescriptorSetLayout> &layouts);
		Semaphore createSemaphore();
		std::vector<Semaphore> createSemaphores(size_t count);
		Fence createFence(vk::FenceCreateFlags flags = {});
		std::vector<Fence> createFences(size_t count, vk::FenceCreateFlags flags = {});
		Sampler createSampler(vk::Filter filter = vk::Filter::eLinear, vk::SamplerAddressMode addressmode = vk::SamplerAddressMode::eRepeat, float maxAnisotropy = 0.0f, vk::BorderColor = vk::BorderColor::eFloatTransparentBlack);


		template<typename ...elements>
		Pipeline createPipeline(const std::vector<vk::PipelineShaderStageCreateInfo> &shaderstages, const PipelineLayout &pipelineLayout, const RenderPass &renderpass, vk::Extent2D extent);

		inline Buffer createStagingBuffer(const PhysicalDevice &physicaldevice, vk::DeviceSize size, vk::BufferUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {}) {
			return createBuffer(physicaldevice, size,
				usage | vk::BufferUsageFlagBits::eTransferSrc,
				sharingMode,
				properties | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
			);
		}
		inline Buffer createIndexBuffer(const PhysicalDevice &physicaldevice, vk::DeviceSize size, vk::BufferUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {}) {
			return createBuffer(physicaldevice, size,
				usage | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
				sharingMode,
				properties | vk::MemoryPropertyFlagBits::eDeviceLocal
			);
		}
		inline Buffer createVertexBuffer(const PhysicalDevice &physicaldevice, vk::DeviceSize size, vk::BufferUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {}) {
			return createBuffer(physicaldevice, size,
				usage | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
				sharingMode,
				properties | vk::MemoryPropertyFlagBits::eDeviceLocal
			);
		}
		template<class UBO>
		Buffer createUniformBuffer(const PhysicalDevice &physicaldevice, vk::BufferUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {}) {
			return createBuffer(physicaldevice, sizeof(UBO),
				usage | vk::BufferUsageFlagBits::eUniformBuffer,
				sharingMode,
				properties | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
			);
		}
		Buffer createUniformBuffer(const PhysicalDevice &physicaldevice, vk::DeviceSize size, vk::BufferUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {}) {
			return createBuffer(physicaldevice, size,
				usage | vk::BufferUsageFlagBits::eUniformBuffer,
				sharingMode,
				properties | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
			);
		}
		inline Image createImage1D(const PhysicalDevice &physicaldevice, vk::Extent3D extent, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {}) {
			return createImage(physicaldevice, extent, format, tiling, vk::ImageType::e1D,
				usage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				sharingMode, properties | vk::MemoryPropertyFlagBits::eDeviceLocal
			);
		}
		inline Image createImage2D(const PhysicalDevice &physicaldevice, vk::Extent2D extent, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {}) {
			return createImage(physicaldevice, {extent.width, extent.height, 1}, format, tiling, vk::ImageType::e2D,
				usage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				sharingMode, properties | vk::MemoryPropertyFlagBits::eDeviceLocal
			);
		}
		inline Image createImage3D(const PhysicalDevice &physicaldevice, vk::Extent3D extent, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage = {}, vk::SharingMode sharingMode = vk::SharingMode::eExclusive, vk::MemoryPropertyFlags properties = {}) {
			return createImage(physicaldevice, extent, format, tiling, vk::ImageType::e3D,
				usage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				sharingMode, properties | vk::MemoryPropertyFlagBits::eDeviceLocal
			);
		}
		inline vk::Queue getGraphicsQueue(uint32_t queueIndex = 0) {
			return getQueue(indices.graphicsFamily.value(), queueIndex);
		}
		inline vk::Queue getPresentQueue(uint32_t queueIndex = 0) {
			return getQueue(indices.presentFamily.value(), queueIndex);
		}
		inline void setQueueFamilyIndices(QueueFamilyIndices indices) {
			this->indices = indices;
		}
		inline QueueFamilyIndices getQueueFamilyIndices() {
			return indices;
		}
		inline vk::Pipeline createGraphicsPipeline(vk::GraphicsPipelineCreateInfo createinfo) {
#if defined(WIN32)
			return createGraphicsPipelines(nullptr, {createinfo}).value[0];
#else
			return createGraphicsPipelines(nullptr, {createinfo}).value[0];
#endif
		}
	private:
		QueueFamilyIndices indices = {};
	};
	typedef vk::UniqueHandle<AdvancedDevice, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> Device;

	template<typename ...elements>
	Pipeline AdvancedDevice::createPipeline(const std::vector<vk::PipelineShaderStageCreateInfo> &shaderstages, const PipelineLayout &pipelineLayout, const RenderPass &renderpass, vk::Extent2D extent) {
		auto bindingDescriptions = photonvk::Vertex<elements...>::getBindingDescriptions();
		auto attributeDescriptions = photonvk::Vertex<elements...>::getAttributeDescriptions();
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo({},
			bindingDescriptions.size(), bindingDescriptions.data(),
			attributeDescriptions.size(), attributeDescriptions.data()
		);
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly({},
			vk::PrimitiveTopology::eTriangleList, false
		);

		std::vector<vk::Viewport> viewports = {vk::Viewport(0, 0, extent.width, extent.height, 0.0f, 1.0f)};
		std::vector<vk::Rect2D> scissors = {vk::Rect2D({0, 0}, extent)};

		vk::PipelineViewportStateCreateInfo viewportState({},
			viewports.size(), viewports.data(), scissors.size(), scissors.data()
		);
		vk::PipelineRasterizationStateCreateInfo rasterizer({},
			false, false, 
			vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise,
			false, 0.0f, 0.0f, 0.0f,
			1.0f
		);
		vk::PipelineMultisampleStateCreateInfo multisampling({},
			vk::SampleCountFlagBits::e1, false, 1.0f, nullptr, false, false
		);
		vk::PipelineDepthStencilStateCreateInfo depthstencil({},
			true, false, vk::CompareOp::eLess,
			false, false, {}, {}, 0.0f, 1.0f
		);
		std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments = {vk::PipelineColorBlendAttachmentState(
			true,
			vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		)};
		vk::PipelineColorBlendStateCreateInfo colorBlending({},
			false, vk::LogicOp::eCopy,
			colorBlendAttachments.size(), colorBlendAttachments.data(),
			{0.0f, 0.0f, 0.0f, 0.0f}
		);

		vk::GraphicsPipelineCreateInfo createinfo({},
			shaderstages.size(), shaderstages.data(),
			&vertexInputInfo,
			&inputAssembly,
			nullptr,
			&viewportState,
			&rasterizer,
			&multisampling,
			&depthstencil,
			&colorBlending,
			nullptr,
			pipelineLayout.get(),
			renderpass.get(), 0,
			nullptr, -1
		);
		vk::Pipeline pipeline = createGraphicsPipeline(createinfo);
		return Pipeline(pipeline, vk::ObjectDestroy<vk::Device, vk::DispatchLoaderStatic>(*this));
	}
}

template <typename Dispatch>
class vk::UniqueHandleTraits<photonvk::AdvancedDevice, Dispatch> {
public:
	using deleter = vk::ObjectDestroy<vk::NoParent, Dispatch>;
};