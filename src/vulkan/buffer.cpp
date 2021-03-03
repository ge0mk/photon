#include <vulkan/buffer.hpp>
#include <math/math.hpp>
#include <iostream>

namespace photonvk {
	void AdvancedBuffer::copyFrom(const AdvancedBuffer &buffer, const CommandBuffer &commandbuffer, const vk::Queue &queue) {
		if(*this && buffer && memory && buffer.memory) {
			vk::CommandBufferBeginInfo begininfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
			std::vector<vk::BufferCopy> regions = {vk::BufferCopy(0, 0, buffer.size)};
			commandbuffer->begin(begininfo);
			commandbuffer->copyBuffer(buffer, *this, regions.size(), regions.data());
			commandbuffer->end();
			vk::SubmitInfo submitinfo(
				0, nullptr, nullptr,
				1, &commandbuffer.get(),
				0, nullptr
			);
			queue.submit({submitinfo}, nullptr);
			queue.waitIdle();
		}
	}
}