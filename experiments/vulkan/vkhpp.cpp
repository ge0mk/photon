#include <vulkan/vulkan.hpp>

int main() {
	vk::ApplicationInfo appinfo("app", 1, "engine", 1, VK_API_VERSION_1_2);
	vk::InstanceCreateInfo instanceCreateInfo({, &appinfo});
	vk::Instance instance = vk::createInstance(instanceCreateInfo);
}