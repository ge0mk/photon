#define GLFW_INCLUDE_VULKAN
#include <GLFW/window.hpp>
#include <photonvk/photonvk.hpp>
#include <spdlog/spdlog.h>
#include <math/matrix.hpp>
#include <utils/image.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void *that) {
	switch(severity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
			spdlog::debug(pCallbackData->pMessage);
		} break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
			spdlog::info(pCallbackData->pMessage);
		} break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
			spdlog::warn(pCallbackData->pMessage);
		} break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
			spdlog::error(pCallbackData->pMessage);
		} break;
	}
	return VK_FALSE;
}

static void check_vk_result(VkResult err) {
	if (err == 0) return;
	printf("VkResult %d\n", err);
	if (err < 0)
		abort();
}

const std::vector<photonvk::Vertex<math::vec2, math::vec3, math::vec2>> vertices = {
	{math::vec2{-0.5f, -0.5f}, math::vec3{1.0f, 0.0f, 0.0f}, math::vec2{0.0f, 0.0f}},
	{math::vec2{ 0.5f, -0.5f}, math::vec3{0.0f, 1.0f, 0.0f}, math::vec2{1.0f, 0.0f}},
	{math::vec2{ 0.5f,  0.5f}, math::vec3{0.0f, 0.0f, 1.0f}, math::vec2{1.0f, 1.0f}},
	{math::vec2{-0.5f,  0.5f}, math::vec3{1.0f, 1.0f, 1.0f}, math::vec2{0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

struct UBO {
	math::mat4 model, view, projection;
};

int main(int argc, char *argv[]) {
	uint32_t width = 1080, height = 720;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfw::Window win(width, height, "test");

	std::vector<const char*> extensions = win.getRequiredExtensions();
	std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

	vk::ApplicationInfo appinfo("testapp", 1, "testengine", 1, VK_API_VERSION_1_2);
	vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo({},
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose /*| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo*/ | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		debugCallback, nullptr
	);

	photonvk::Instance instance = photonvk::createInstance(appinfo, extensions, layers, debugMessengerInfo);
	photonvk::InstanceDispatchLoader instanceDispatchLoader = instance->createDispatchLoader();
	photonvk::DebugUtilsMessenger debugMessenger = instance->createDebugUtilsMessengerEXTUnique(debugMessengerInfo, nullptr, instanceDispatchLoader);
	photonvk::Surface surface = win.createSurface(instance);
	photonvk::PhysicalDevice physicaldevice = instance->pickPhysicalDevice([&deviceExtensions, &surface](photonvk::PhysicalDevice dev) -> float {
		if(dev.checkExtensionSupport(deviceExtensions)) {
			vk::PhysicalDeviceProperties deviceProperties = dev.getProperties();
			vk::PhysicalDeviceFeatures deviceFeatures = dev.getFeatures();

			auto indices = dev.findQueueFamilies(surface);
			auto swapChainSupport = dev.querySwapChainSupport(surface);
			bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			if(indices && swapChainAdequate) {
				return 1.0f + (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu); // prefer discrete gpu's over integrated ones
			}
		}
		return 0.0f;
	});
	vk::Format depthBufferFormat = physicaldevice.findDepthBufferFormat();
	photonvk::Device device = physicaldevice.createDevice(surface, deviceExtensions);
	photonvk::QueueFamilyIndices queueFamilyIndices = device->getQueueFamilyIndices();
	vk::Queue graphicsQueue = device->getGraphicsQueue();
	vk::Queue presentQueue = device->getPresentQueue();
	photonvk::Swapchain swapchain = device->createSwapchain(physicaldevice, surface, {width, height});
	uint32_t imagecount = device->getSwapchainImageCount(swapchain);
	photonvk::RenderPass renderpass = device->createRenderPass(swapchain->getImageFormat(), depthBufferFormat);
	photonvk::RenderPass uiRenderPass = device->createRenderPass(swapchain->getImageFormat(), depthBufferFormat, vk::ImageLayout::ePresentSrcKHR);
	photonvk::DescriptorSetLayout descriptorSetLayout = device->createDescriptorSetLayout({
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex),
		vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment)
	});
	photonvk::ShaderModule vertexshader = device->loadShaderModule("vert.spv");
	photonvk::ShaderModule fragmentshader = device->loadShaderModule("frag.spv");
	std::vector<vk::PipelineShaderStageCreateInfo> shaderstages = {
		vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertexshader.get(), "main"),
		vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragmentshader.get(), "main")
	};
	photonvk::PipelineLayout pipelineLayout = device->createPipelineLayout({descriptorSetLayout.get()});
	photonvk::Pipeline pipeline = device->createPipeline<math::vec2, math::vec3, math::vec2>(shaderstages, pipelineLayout, renderpass, swapchain->getExtent());
	photonvk::CommandPool commandpool = device->createCommandPool(queueFamilyIndices.graphicsFamily.value(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	photonvk::DescriptorPool descriptorpool = device->createDescriptorPool({{vk::DescriptorType::eUniformBuffer, imagecount}}, 1000, vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

	photonvk::DescriptorPool imguiDescriptorPool = device->createDescriptorPool({
		{ vk::DescriptorType::eSampler, 1000 },
		{ vk::DescriptorType::eCombinedImageSampler, 1000 },
		{ vk::DescriptorType::eSampledImage, 1000 },
		{ vk::DescriptorType::eStorageImage, 1000 },
		{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
		{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
		{ vk::DescriptorType::eUniformBuffer, 1000 },
		{ vk::DescriptorType::eStorageBuffer, 1000 },
		{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
		{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
		{ vk::DescriptorType::eInputAttachment, 1000 }
	}, 1000 * 11, vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);


	photonvk::CommandBuffer copyCommandBuffer = device->allocateCommandBuffer(commandpool);
	size_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
	photonvk::Buffer stagingBuffer = device->createStagingBuffer(physicaldevice, vertexBufferSize);
	device->setBufferData(stagingBuffer, vertices.data(), vertexBufferSize);
	photonvk::Buffer vertexBuffer = device->createVertexBuffer(physicaldevice, vertexBufferSize);
	vertexBuffer->copyFrom(stagingBuffer, copyCommandBuffer, graphicsQueue);

	copyCommandBuffer->reset({});
	size_t indexBufferSize = sizeof(indices[0]) * indices.size();
	stagingBuffer = device->createStagingBuffer(physicaldevice, indexBufferSize);
	device->setBufferData(stagingBuffer, indices.data(), indexBufferSize);
	photonvk::Buffer indexBuffer = device->createIndexBuffer(physicaldevice, indexBufferSize);
	indexBuffer->copyFrom(stagingBuffer, copyCommandBuffer, graphicsQueue);

	copyCommandBuffer->reset({});
	Image img("textures/debug_uv_02.png");
	stagingBuffer = device->createStagingBuffer(physicaldevice, img.size().x * img.size().y * img.channels());
	device->setBufferData(stagingBuffer, img.data(), img.size().x * img.size().y * img.channels());
	photonvk::Image texture = device->createImage2D(physicaldevice, {uint32_t(img.size().x), uint32_t(img.size().y)}, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal);
	texture->transitionLayout(copyCommandBuffer, graphicsQueue, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	copyCommandBuffer->reset({});
	texture->copyFrom(stagingBuffer, copyCommandBuffer, graphicsQueue);
	copyCommandBuffer->reset({});
	texture->transitionLayout(copyCommandBuffer, graphicsQueue, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	photonvk::ImageView textureView = device->createImageView(texture.get(), vk::Format::eR8G8B8A8Srgb);
	photonvk::Sampler sampler = device->createSampler(vk::Filter::eLinear);

	std::vector<photonvk::Buffer> uniformBuffers(imagecount);
	for(unsigned i = 0; i < imagecount; i++) {
		uniformBuffers[i] = device->createUniformBuffer<UBO>(physicaldevice);
	}
	auto layouts = std::vector<vk::DescriptorSetLayout>(imagecount, descriptorSetLayout.get());
	std::vector<photonvk::DescriptorSet> descriptorsets = device->createDescriptorSets(descriptorpool, layouts);
	for(unsigned i = 0; i < imagecount; i++) {
		vk::DescriptorBufferInfo bufferinfo(
			uniformBuffers[i].get(), 0, uniformBuffers[i]->size
		);
		vk::DescriptorImageInfo imageinfo(
			sampler.get(), textureView.get(), vk::ImageLayout::eShaderReadOnlyOptimal
		);
		vk::WriteDescriptorSet uboWrite(
			descriptorsets[i].get(), 0, 0,
			1, vk::DescriptorType::eUniformBuffer,
			nullptr, &bufferinfo, nullptr
		);
		vk::WriteDescriptorSet texWrite(
			descriptorsets[i].get(), 1, 0,
			1, vk::DescriptorType::eCombinedImageSampler,
			&imageinfo, nullptr, nullptr
		);
		device->updateDescriptorSets({uboWrite, texWrite}, 0);
	}
	std::vector<photonvk::CommandBuffer> commandBuffers = device->allocateCommandBuffers(commandpool, imagecount);
	std::vector<photonvk::CommandBuffer> uiCommandBuffers = device->allocateCommandBuffers(commandpool, imagecount);
	std::vector<photonvk::Semaphore> imageAvailableSemaphores = device->createSemaphores(imagecount);
	std::vector<photonvk::Semaphore> renderFinishedSemaphores = device->createSemaphores(imagecount);
	std::vector<photonvk::Fence> fences = device->createFences(imagecount, vk::FenceCreateFlagBits::eSignaled);

	std::vector<vk::Image> swapchainImages;
	std::vector<photonvk::ImageView> imageViews;
	std::vector<photonvk::Framebuffer> framebuffers;
	std::vector<vk::Extent2D> extents;
	photonvk::Image depthBuffer;
	photonvk::ImageView depthBufferView;

	auto recreateFrame = [&](unsigned i) {
		extents[i] = swapchain->getExtent();
		framebuffers[i] = device->createFramebuffer({imageViews[i].get(), depthBufferView.get()}, extents[i], renderpass);
		vk::CommandBufferBeginInfo begininfo;
		std::vector<vk::ClearValue> clearColors = {
			vk::ClearColorValue(std::array<float, 4>{0.025f, 0.03f, 0.035f, 0.0f}),
			vk::ClearDepthStencilValue(1.0f, 0.0f)
		};
		vk::RenderPassBeginInfo renderPassInfo(
			renderpass.get(), framebuffers[i].get(),
			vk::Rect2D({0, 0}, extents[i]),
			clearColors.size(), clearColors.data()
		);
		vk::Buffer vertexBuffers[] = {vertexBuffer.get()};
		vk::DeviceSize offsets[] = {0};

		photonvk::CommandBuffer &buffer = commandBuffers[i];
		buffer->reset({});
		buffer->begin(begininfo);
		buffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
		buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
		buffer->bindVertexBuffers(0, 1, vertexBuffers, offsets);
		buffer->bindIndexBuffer(indexBuffer.get(), 0, vk::IndexType::eUint16);
		buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout.get(), 0, 1, &descriptorsets[i].get(), 0, nullptr);
		buffer->drawIndexed(indices.size(), 1, 0, 0, 0);
		buffer->endRenderPass();
		buffer->end();
	};
	auto recreateFrames = [&]() {
		copyCommandBuffer->reset({});
		depthBuffer = device->createImage2D(physicaldevice, swapchain->getExtent(), depthBufferFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal);
		depthBuffer->transitionLayout(copyCommandBuffer, graphicsQueue, depthBufferFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
		depthBufferView = device->createImageView(depthBuffer.get(), depthBufferFormat, vk::ImageAspectFlagBits::eDepth);
		swapchainImages = device->getSwapchainImagesKHR(swapchain.get());
		imageViews = device->createImageViews(swapchainImages, swapchain->getImageFormat());
		framebuffers.resize(imagecount);
		extents.resize(imagecount);
	};
	recreateFrames();
	auto recreateSwapchain = [&]() {
		int w, h;
		glfwGetFramebufferSize(win.getHandle(), &w, &h);
		device->waitIdle();
		width = uint32_t(w), height = uint32_t(h);

		swapchain = device->recreateSwapchain(swapchain, physicaldevice, surface, {width, height});
		renderpass = device->createRenderPass(swapchain->getImageFormat(), depthBufferFormat);
		uiRenderPass = device->createRenderPass(swapchain->getImageFormat(), depthBufferFormat);
		pipeline = device->createPipeline<math::vec2, math::vec3, math::vec2>(shaderstages, pipelineLayout, renderpass, swapchain->getExtent());
		recreateFrames();
		device->waitIdle();
	};

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForVulkan(win.getHandle(), true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance.get();
	init_info.PhysicalDevice = physicaldevice;
	init_info.Device = device.get();
	init_info.QueueFamily = queueFamilyIndices.graphicsFamily.value();
	init_info.Queue = graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = imguiDescriptorPool.get();
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 2;
	init_info.ImageCount = imagecount;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, uiRenderPass.get());

	// Upload Fonts
	{
		copyCommandBuffer->reset({});
		vk::CommandBufferBeginInfo begininfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		copyCommandBuffer->begin(begininfo);
		ImGui_ImplVulkan_CreateFontsTexture(copyCommandBuffer.get());
		copyCommandBuffer->end();
		vk::SubmitInfo submitinfo(
			0, nullptr, nullptr,
			1, &copyCommandBuffer.get(),
			0, nullptr
		);
		graphicsQueue.submit({submitinfo}, nullptr);
		device->waitIdle();
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	// set event callback
	win.setEventCallback([&](Event *event){
		dispatch<ResizeEvent>(event, [&](ResizeEvent *event){
			recreateSwapchain();
		});
	});

	unsigned frame = 0;
	// gameloop
	while(!win.windowShouldClose()) {
		win.pollEvents();
		//render ui
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Hello World!");
		ImGui::End();
		ImGui::Render();
		//render
		if(extents[frame] != swapchain->getExtent()) {
			recreateFrame(frame);
		}
		UBO ubo = {
			math::mat4().translate(math::vec3(0,0,1)).scale(2),
			math::mat4(),
			math::mat4::projection(3.1415/2.0, float(width) / float(height), 0.1, 10)
		};
		device->setBufferData(uniformBuffers[frame], &ubo, sizeof(ubo));
		device->waitForFences({fences[frame].get()}, true, UINT64_MAX);
		auto r = device->acquireNextImageKHR(swapchain.get(), UINT32_MAX, imageAvailableSemaphores[frame].get(), nullptr);
		if(r.result == vk::Result::eErrorOutOfDateKHR || r.result == vk::Result::eSuboptimalKHR) {
			recreateSwapchain();
			frame = 0;
			continue;
		}
		else if(r.result != vk::Result::eSuccess) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		uint32_t image = r.value;

		vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[frame].get()};
		vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[frame].get()};
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

		{
			vk::CommandBufferBeginInfo begininfo;
			std::vector<vk::ClearValue> clearColors = {
				vk::ClearColorValue(std::array<float, 4>{0.025f, 0.03f, 0.035f, 0.0f}),
				vk::ClearDepthStencilValue(1.0f, 0.0f)
			};
			vk::RenderPassBeginInfo uiRenderPassInfo(
				uiRenderPass.get(), framebuffers[frame].get(),
				vk::Rect2D({0, 0}, extents[frame]),
				clearColors.size(), clearColors.data()
			);
			uiCommandBuffers[frame]->reset({});
			uiCommandBuffers[frame]->begin(begininfo);
			uiCommandBuffers[frame]->beginRenderPass(uiRenderPassInfo, vk::SubpassContents::eInline);
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), uiCommandBuffers[frame].get());
			uiCommandBuffers[frame]->endRenderPass();
			uiCommandBuffers[frame]->end();
		}

		std::vector<vk::CommandBuffer> renderCommandBuffers = {commandBuffers[frame].get(), uiCommandBuffers[frame].get()};

		vk::SubmitInfo submitinfo(
			1, waitSemaphores, waitStages,
			renderCommandBuffers.size(), renderCommandBuffers.data(),
			1, signalSemaphores
		);
		device->resetFences({fences[frame].get()});
		graphicsQueue.submit({submitinfo}, fences[frame].get());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		vk::SwapchainKHR swapchains[] = {swapchain.get()};
		vk::PresentInfoKHR presentInfo(
			1, signalSemaphores,
			1, swapchains,
			&image, nullptr
		);
		auto result = presentQueue.presentKHR(&presentInfo);
		if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
			recreateSwapchain();
			frame = 0;
			continue;
		}
		else if(result != vk::Result::eSuccess) {
			throw std::runtime_error("failed to present swap chain image!");
		}
		frame = (frame + 1) % imagecount;
	}
	device->waitIdle();
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}