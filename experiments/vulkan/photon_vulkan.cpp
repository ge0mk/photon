#include "photon.hpp"
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>

VKAPI_ATTR VkBool32 VKAPI_CALL Photon::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void *that) {
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
		default: break;
	}
	return VK_FALSE;
}

void Photon::initVulkan() {
	std::vector<const char*> extensions = win.requiredExtensions();
	std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	std::vector<const char*> layers = {};//{"VK_LAYER_KHRONOS_validation"};

	vk::ApplicationInfo appinfo("testapp", 1, "testengine", 1, VK_API_VERSION_1_2);
	vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo({},
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose /*| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo*/ | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		Photon::debugCallback, this
	);

	instance = photonvk::createInstance(appinfo, extensions, layers);
	instanceDispatchLoader = instance->createDispatchLoader();
	//debugMessenger = instance->createDebugUtilsMessengerEXTUnique(debugMessengerInfo, nullptr, instanceDispatchLoader);
	surface = win.createSurface(instance);
	physicaldevice = instance->pickPhysicalDevice([&](photonvk::PhysicalDevice dev) -> float {
		if(dev.checkExtensionSupport(deviceExtensions)) {
			vk::PhysicalDeviceProperties deviceProperties = dev.getProperties();
			vk::PhysicalDeviceFeatures deviceFeatures = dev.getFeatures();

			auto indices = dev.findQueueFamilies(surface);
			auto swapChainSupport = dev.querySwapChainSupport(surface, instance.get());
			bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			if(indices && swapChainAdequate) {
				return 1.0f + (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu); // prefer discrete gpu's over integrated ones
			}
		}
		return 0.0f;
	});
	depthBufferFormat = physicaldevice.findDepthBufferFormat();
	device = physicaldevice.createDevice(surface, deviceExtensions, {}, instance.get());
	queueFamilyIndices = device->getQueueFamilyIndices();
	graphicsQueue = device->getGraphicsQueue();
	presentQueue = device->getPresentQueue();
	swapchain = device->createSwapchain(physicaldevice, surface, {1080, 720});
	imagecount = device->getSwapchainImageCount(swapchain);
	renderpass = device->createRenderPass(swapchain->getImageFormat(), depthBufferFormat);
	uiRenderPass = device->createRenderPass(swapchain->getImageFormat(), depthBufferFormat, vk::ImageLayout::ePresentSrcKHR);
	descriptorSetLayout = device->createDescriptorSetLayout({
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex),
		vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eVertex),
		vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment)
	});
	vertexshader = device->loadShaderModule("vert.spv");
	fragmentshader = device->loadShaderModule("frag.spv");
	shaderstages = {
		vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertexshader.get(), "main"),
		vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragmentshader.get(), "main")
	};
	pipelineLayout = device->createPipelineLayout({descriptorSetLayout.get()});
	pipeline = device->createPipeline<math::vec3, math::vec3, math::vec2>(shaderstages, pipelineLayout, renderpass, swapchain->getExtent());
	commandpool = device->createCommandPool(queueFamilyIndices.graphicsFamily.value(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	descriptorpool = device->createDescriptorPool({{vk::DescriptorType::eUniformBuffer, imagecount}}, 1000, vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

	photonvk::CommandBuffer copyCommandBuffer = device->allocateCommandBuffer(commandpool);
	utils::image img("textures/axes.png", true);
	photonvk::Buffer stagingBuffer = device->createStagingBuffer(physicaldevice, img.size());
	device->setBufferData(stagingBuffer, img.data(), img.size());
	texture = device->createImage2D(physicaldevice, {uint32_t(img.width()), uint32_t(img.height())}, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal);
	texture->transitionLayout(copyCommandBuffer, graphicsQueue, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	copyCommandBuffer->reset({});
	texture->copyFrom(stagingBuffer, copyCommandBuffer, graphicsQueue);
	copyCommandBuffer->reset({});
	texture->transitionLayout(copyCommandBuffer, graphicsQueue, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	textureView = device->createImageView(texture.get(), vk::Format::eR8G8B8A8Srgb);
	sampler = device->createSampler(vk::Filter::eNearest);

	copyCommandBuffer->reset({});
	depthBuffer = device->createImage2D(physicaldevice, swapchain->getExtent(), depthBufferFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal);
	depthBuffer->transitionLayout(copyCommandBuffer, graphicsQueue, depthBufferFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	depthBufferView = device->createImageView(depthBuffer.get(), depthBufferFormat, vk::ImageAspectFlagBits::eDepth);

	uint32_t minUboAlignment = physicaldevice.getProperties().limits.minUniformBufferOffsetAlignment;
	uint32_t dynamicAlignment = sizeof(math::mat4);
	if (minUboAlignment > 0) {
		dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}

	uniformBuffers.resize(imagecount);
	dynamicUniformBuffers.resize(imagecount);
	for(unsigned i = 0; i < imagecount; i++) {
		uniformBuffers[i] = device->createUniformBuffer<UniformBufferObject>(physicaldevice);
	}

	commandBuffers = device->allocateCommandBuffers(commandpool, imagecount);
	imageAvailableSemaphores = device->createSemaphores(imagecount);
	renderFinishedSemaphores = device->createSemaphores(imagecount);
	fences = device->createFences(imagecount, vk::FenceCreateFlagBits::eSignaled);

	createDescriptorSets();
	recreateSwapchainImages();
	for(unsigned i = 0; i < imagecount; i++) {
		recreateFrame(i);
	}

	addMeshToScene(createMeshIndexed(vertices, indices));
}

void Photon::createDescriptorSets() {
	uint32_t minUboAlignment = physicaldevice.getProperties().limits.minUniformBufferOffsetAlignment;
	uint32_t dynamicAlignment = sizeof(math::mat4);
	if (minUboAlignment > 0) {
		dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}
	std::vector<vk::DescriptorSetLayout> layouts = std::vector<vk::DescriptorSetLayout>(imagecount, descriptorSetLayout.get());
	descriptorsets = device->createDescriptorSets(descriptorpool, layouts);
	for(unsigned i = 0; i < imagecount; i++) {
		dynamicUniformBuffers[i] = device->createUniformBuffer(physicaldevice, dynamicAlignment * std::max(int(scene.size()), 1));

		vk::DescriptorBufferInfo bufferinfo(
			uniformBuffers[i].get(), 0, uniformBuffers[i]->size
		);
		vk::DescriptorBufferInfo dynamicbufferinfo(
			dynamicUniformBuffers[i].get(), 0, dynamicAlignment
		);
		vk::DescriptorImageInfo imageinfo(
			sampler.get(), textureView.get(), vk::ImageLayout::eShaderReadOnlyOptimal
		);
		vk::WriteDescriptorSet uboWrite(
			descriptorsets[i].get(), 0, 0,
			1, vk::DescriptorType::eUniformBuffer,
			nullptr, &bufferinfo, nullptr
		);
		vk::WriteDescriptorSet dynamicUboWrite(
			descriptorsets[i].get(), 1, 0,
			1, vk::DescriptorType::eUniformBufferDynamic,
			nullptr, &dynamicbufferinfo, nullptr
		);
		vk::WriteDescriptorSet texWrite(
			descriptorsets[i].get(), 2, 0,
			1, vk::DescriptorType::eCombinedImageSampler,
			&imageinfo, nullptr, nullptr
		);
		device->updateDescriptorSets({uboWrite, dynamicUboWrite, texWrite}, 0);
	}
}

void Photon::initImgui() {
	uiDescriptorPool = device->createDescriptorPool({
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
	uiCommandBuffers = device->allocateCommandBuffers(commandpool, imagecount);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForVulkan(win.getHandle(), false);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance.get();
	init_info.PhysicalDevice = physicaldevice;
	init_info.Device = device.get();
	init_info.QueueFamily = queueFamilyIndices.graphicsFamily.value();
	init_info.Queue = graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = uiDescriptorPool.get();
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 2;
	init_info.ImageCount = imagecount;
	init_info.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&init_info, uiRenderPass.get());
}

void Photon::initImguiFonts() {
	photonvk::CommandBuffer copyCommandBuffer = device->allocateCommandBuffer(commandpool);
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

void Photon::cleanImgui() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Photon::recreateCommandbuffer(unsigned index) {
	vk::CommandBufferBeginInfo begininfo;
	std::vector<vk::ClearValue> clearColors = {
		vk::ClearColorValue(std::array<float, 4>{0.025f, 0.03f, 0.035f, 0.0f}),
		vk::ClearDepthStencilValue(1.0f, 0.0f)
	};
	vk::RenderPassBeginInfo renderPassInfo(
		renderpass.get(), framebuffers[index].get(),
		vk::Rect2D({0, 0}, extents[index]),
		clearColors.size(), clearColors.data()
	);

	uint32_t minUboAlignment = physicaldevice.getProperties().limits.minUniformBufferOffsetAlignment;
	uint32_t dynamicAlignment = sizeof(math::mat4);
	if (minUboAlignment > 0) {
		dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}

	photonvk::CommandBuffer &buffer = commandBuffers[index];
	buffer->reset({});
	buffer->begin(begininfo);
	buffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
	for(unsigned i = 0; i < scene.size(); i++) {
		buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout.get(), 0, {descriptorsets[index].get()}, {dynamicAlignment * i});
		scene[i]->render(buffer);
	}
	buffer->endRenderPass();
	buffer->end();
}

void Photon::recreateFrame(unsigned index) {
	extents[index] = swapchain->getExtent();
	framebuffers[index] = device->createFramebuffer({imageViews[index].get(), depthBufferView.get()}, extents[index], renderpass);
	recreateCommandbuffer(index);
}

void Photon::recreateSwapchainImages() {
	photonvk::CommandBuffer copyCommandBuffer = device->allocateCommandBuffer(commandpool);
	depthBuffer = device->createImage2D(physicaldevice, swapchain->getExtent(), depthBufferFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal);
	depthBuffer->transitionLayout(copyCommandBuffer, graphicsQueue, depthBufferFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	depthBufferView = device->createImageView(depthBuffer.get(), depthBufferFormat, vk::ImageAspectFlagBits::eDepth);
	swapchainImages = device->getSwapchainImagesKHR(swapchain.get(), device->dispatchLoader);
	imageViews = device->createImageViews(swapchainImages, swapchain->getImageFormat());
	framebuffers.resize(imagecount);
	extents.resize(imagecount);
}

void Photon::recreateSwapchain() {
	vk::Extent2D framebufferExtent = win.getFramebufferExtent();
	device->waitIdle();

	swapchain = device->recreateSwapchain(swapchain, physicaldevice, surface, framebufferExtent);
	renderpass = device->createRenderPass(swapchain->getImageFormat(), depthBufferFormat);
	uiRenderPass = device->createRenderPass(swapchain->getImageFormat(), depthBufferFormat);
	pipeline = device->createPipeline<math::vec3, math::vec3, math::vec2>(shaderstages, pipelineLayout, renderpass, swapchain->getExtent());
	recreateSwapchainImages();
	device->waitIdle();
}

std::shared_ptr<photonvk::Mesh> Photon::createMesh(std::vector<Vertex> vertices) {
	photonvk::CommandBuffer copyCommandBuffer = device->allocateCommandBuffer(commandpool);
	size_t vertexBufferSize = sizeof(Vertex) * vertices.size();	
	photonvk::Buffer stagingBuffer = device->createStagingBuffer(physicaldevice, vertexBufferSize);
	device->setBufferData(stagingBuffer, vertices.data(), vertexBufferSize);
	photonvk::Buffer vertexBuffer = device->createVertexBuffer(physicaldevice, vertexBufferSize);
	vertexBuffer->copyFrom(stagingBuffer, copyCommandBuffer, graphicsQueue);

	return std::make_shared<photonvk::Mesh>(std::move(vertexBuffer), vertices.size());
}

std::shared_ptr<photonvk::Mesh> Photon::createMeshIndexed(std::vector<Vertex> vertices, std::vector<uint16_t> indices) {
	photonvk::CommandBuffer copyCommandBuffer = device->allocateCommandBuffer(commandpool);
	size_t vertexBufferSize = sizeof(Vertex) * vertices.size();	
	photonvk::Buffer stagingBuffer = device->createStagingBuffer(physicaldevice, vertexBufferSize);
	device->setBufferData(stagingBuffer, vertices.data(), vertexBufferSize);
	photonvk::Buffer vertexBuffer = device->createVertexBuffer(physicaldevice, vertexBufferSize);
	vertexBuffer->copyFrom(stagingBuffer, copyCommandBuffer, graphicsQueue);

	copyCommandBuffer->reset({});
	size_t indexBufferSize = sizeof(uint16_t) * indices.size();
	stagingBuffer = device->createStagingBuffer(physicaldevice, indexBufferSize);
	device->setBufferData(stagingBuffer, indices.data(), indexBufferSize);
	photonvk::Buffer indexBuffer = device->createIndexBuffer(physicaldevice, indexBufferSize);
	indexBuffer->copyFrom(stagingBuffer, copyCommandBuffer, graphicsQueue);
	
	return std::make_shared<photonvk::IndexedMesh>(std::move(vertexBuffer), std::move(indexBuffer), indices.size());
}

void Photon::addMeshToScene(std::shared_ptr<photonvk::Mesh> mesh) {
	if(mesh.get()) {
		device->waitIdle();
		scene.push_back(mesh);
		createDescriptorSets();
		for(unsigned i = 0; i < imagecount; i++) {
			recreateCommandbuffer(i);
		}
	}
	else {
		std::cout<<"couldn't add mesh to scene\n";
		throw std::runtime_error("can't add nullptr to scene!");
	}
}

void Photon::removeMeshFromScene(std::shared_ptr<photonvk::Mesh> mesh) {
	device->waitIdle();
	scene.erase(std::remove(scene.begin(), scene.end(), mesh), scene.end());
	createDescriptorSets();
	for(unsigned i = 0; i < imagecount; i++) {
		recreateCommandbuffer(i);
	}
}

void Photon::render(UniformBufferObject ubo) {
	static unsigned frame = 0;
	// render ui
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	renderUI();
	ImGui::Render();
	// render scene
	if(extents[frame] != swapchain->getExtent()) {
		recreateFrame(frame);
	}
	device->setBufferData(uniformBuffers[frame], &ubo, sizeof(ubo));
	std::vector<math::mat4> m(scene.size() * 4);
	for(unsigned i = 0; i < scene.size(); i++) {
		m[i*4] = scene[i]->transform;
	}
	device->setBufferData(dynamicUniformBuffers[frame], m.data(), m.size() * sizeof(math::mat4));
	device->waitForFences({fences[frame].get()}, true, UINT64_MAX);
	auto r = device->acquireNextImageKHR(swapchain.get(), UINT32_MAX, imageAvailableSemaphores[frame].get(), nullptr, device->dispatchLoader);
	if(r.result == vk::Result::eErrorOutOfDateKHR || r.result == vk::Result::eSuboptimalKHR) {
		recreateSwapchain();
		frame = 0;
		return;
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

	if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	vk::SwapchainKHR swapchains[] = {swapchain.get()};
	vk::PresentInfoKHR presentInfo(
		1, signalSemaphores,
		1, swapchains,
		&image, nullptr
	);
	auto result = presentQueue.presentKHR(&presentInfo, device->dispatchLoader);
	if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
		recreateSwapchain();
		frame = 0;
		return;
	}
	else if(result != vk::Result::eSuccess) {
		throw std::runtime_error("failed to present swap chain image!");
	}
	frame = (frame + 1) % imagecount;
}