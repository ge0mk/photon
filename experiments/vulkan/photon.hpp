#pragma once
// photon game engine

#define GLFW_INCLUDE_VULKAN
#include <GLFW/window.hpp>
#include <photonvk/photonvk.hpp>
#include <imgui/imgui.h>
#include <imgui/ImNodes.h>
#include <imgui/ImNodesEz.h>

#include <angelscript/angelscript.h>
#include <angelscript/plugins/all.hpp>

#include <spdlog/spdlog.h>
#include <math/matrix.hpp>
#include <math/vector.hpp>
#include <math/quaternion.hpp>
#include <utils/image.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <functional>
#include <optional>
#include <set>
#include <stdexcept>
#include <thread>

#include <cassert>
#include <cstring>
#include <cstdint>

using namespace std::chrono_literals;

class Photon {
	using Vertex = photonvk::Vertex<math::vec3, math::vec3, math::vec2>;
public: // structs / types
	struct UniformBufferObject {
		math::mat4 view, projection;
	};

public: // member functions
	Photon(math::ivec2 size = math::ivec2(1080, 720), std::string title = "Photon");
	~Photon();
	int run();
	void handle(Event *event);
	virtual void renderUI();

	// scripting
	void initAngelscript();
	void loadScript(std::string file, std::string module, bool runmain = false);
	void createScriptThread(asIScriptFunction *func);
	void scriptPrint(std::string val);
	void scriptPrint(int32_t val);
	void scriptPrint(uint32_t val);
	void scriptPrint(int64_t val);
	void scriptPrint(uint64_t val);
	void scriptPrint(double val);
	void scriptPrint(math::vec2 val);
	void scriptPrint(math::vec3 val);
	void scriptPrint(math::vec4 val);
	void scriptPrint(math::mat4& val);
	void scriptMessageCallback(const asSMessageInfo *msg);
	float getInput(std::string name);
	void setMouseVisible(bool visible);
	bool getMouseVisible();
	std::shared_ptr<photonvk::Mesh> createMesh(CScriptArray &vertices);
	std::shared_ptr<photonvk::Mesh> createMeshIndexed(CScriptArray &vertices, CScriptArray &indices);

	// graphics functions
	void initVulkan();
	void initImgui();
	void initImguiFonts();
	void recreateCommandbuffer(unsigned index);
	void recreateFrame(unsigned index);
	void recreateSwapchainImages();
	void recreateSwapchain();
	void createDescriptorSets();
	void render(UniformBufferObject ubo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void *that);
	void cleanImgui();
	std::shared_ptr<photonvk::Mesh> createMesh(std::vector<Vertex> vertices);
	std::shared_ptr<photonvk::Mesh> createMeshIndexed(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
	void addMeshToScene(std::shared_ptr<photonvk::Mesh> mesh);
	void removeMeshFromScene(std::shared_ptr<photonvk::Mesh> mesh);

protected:
	Window win;

	photonvk::Instance instance;
	photonvk::InstanceDispatchLoader instanceDispatchLoader;
	photonvk::DebugUtilsMessenger debugMessenger;
	photonvk::Surface surface;
	photonvk::PhysicalDevice physicaldevice;
	photonvk::Device device;
	photonvk::QueueFamilyIndices queueFamilyIndices;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;
	photonvk::Swapchain swapchain;
	uint32_t imagecount;
	photonvk::RenderPass renderpass, uiRenderPass;
	photonvk::DescriptorSetLayout descriptorSetLayout;
	photonvk::ShaderModule vertexshader;
	photonvk::ShaderModule fragmentshader;
	std::vector<vk::PipelineShaderStageCreateInfo> shaderstages;
	photonvk::PipelineLayout pipelineLayout;
	photonvk::Pipeline pipeline;
	photonvk::CommandPool commandpool;
	photonvk::DescriptorPool descriptorpool, uiDescriptorPool;
	photonvk::Image texture;
	photonvk::ImageView textureView;
	photonvk::Sampler sampler;
	vk::Format depthBufferFormat;
	photonvk::Image depthBuffer;
	photonvk::ImageView depthBufferView;
	//photonvk::Mesh origin;

	std::vector<photonvk::Buffer> uniformBuffers;
	std::vector<photonvk::Buffer> dynamicUniformBuffers;
	std::vector<photonvk::DescriptorSet> descriptorsets;
	std::vector<photonvk::CommandBuffer> commandBuffers, uiCommandBuffers;
	std::vector<photonvk::Semaphore> imageAvailableSemaphores;
	std::vector<photonvk::Semaphore> renderFinishedSemaphores;
	std::vector<photonvk::Fence> fences;
	std::vector<vk::Image> swapchainImages;
	std::vector<photonvk::ImageView> imageViews;
	std::vector<photonvk::Framebuffer> framebuffers;
	std::vector<vk::Extent2D> extents;

	const std::vector<Vertex> vertices = {
		{math::vec3{-1.0f, -1.0f, 0.0f}, math::vec3{1.0f, 0.0f, 0.0f}, math::vec2{0.0f, 0.0f}},
		{math::vec3{ 1.0f, -1.0f, 0.0f}, math::vec3{1.0f, 1.0f, 0.0f}, math::vec2{1.0f, 0.0f}},
		{math::vec3{ 1.0f,  1.0f, 0.0f}, math::vec3{1.0f, 1.0f, 0.0f}, math::vec2{1.0f, 1.0f}},
		{math::vec3{-1.0f,  1.0f, 0.0f}, math::vec3{1.0f, 0.0f, 0.0f}, math::vec2{0.0f, 1.0f}},
		{math::vec3{-1.0f, 0.0f, -1.0f}, math::vec3{0.0f, 1.0f, 0.0f}, math::vec2{0.0f, 0.0f}},
		{math::vec3{-1.0f, 0.0f,  1.0f}, math::vec3{0.0f, 1.0f, 1.0f}, math::vec2{1.0f, 0.0f}},
		{math::vec3{ 1.0f, 0.0f,  1.0f}, math::vec3{0.0f, 1.0f, 1.0f}, math::vec2{1.0f, 1.0f}},
		{math::vec3{ 1.0f, 0.0f, -1.0f}, math::vec3{0.0f, 1.0f, 0.0f}, math::vec2{0.0f, 1.0f}},
		{math::vec3{0.0f, -1.0f, -1.0f}, math::vec3{0.0f, 0.0f, 1.0f}, math::vec2{0.0f, 0.0f}},
		{math::vec3{0.0f,  1.0f, -1.0f}, math::vec3{1.0f, 0.0f, 1.0f}, math::vec2{1.0f, 0.0f}},
		{math::vec3{0.0f,  1.0f,  1.0f}, math::vec3{1.0f, 0.0f, 1.0f}, math::vec2{1.0f, 1.0f}},
		{math::vec3{0.0f, -1.0f,  1.0f}, math::vec3{0.0f, 0.0f, 1.0f}, math::vec2{0.0f, 1.0f}}
	};
	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8
	};

	std::vector<std::shared_ptr<photonvk::Mesh>> scene;
	
	// controls
	std::map<std::string, uint32_t> keycontrols;
	bool keys[GLFW_KEY_LAST], mouseVisible;
	math::vec2 dmouse = 0, dscroll = 0;

	//scripting:
	asIScriptEngine *engine;
	CContextMgr contextManager;
	CScriptBuilder builder;

	//camera
	struct Camera {
		asIScriptObject *obj = nullptr;
		asITypeInfo *type = nullptr;
		asIScriptContext *ctx = nullptr;
		void init(asIScriptModule *module);
		void deinit();
		void update(float dt, math::vec2 dmouse);
		math::mat4 getViewTransform();
		math::mat4 getProjectionMatrix();
	} cam;
	//gui
	struct ScriptGUI {
		asIScriptObject *obj = nullptr;
		asITypeInfo *type = nullptr;
		asIScriptContext *ctx = nullptr;
		void init(asIScriptModule *module);
		void deinit();
		void render();
	} gui;
};