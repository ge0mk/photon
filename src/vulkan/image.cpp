#include <vulkan/image.hpp>
#include <math/math.hpp>
#include <iostream>

namespace photonvk {
	bool hasStencilComponent(vk::Format format) {
		return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
	}
	void AdvancedImage::copyFrom(const AdvancedBuffer &buffer, const CommandBuffer &commandbuffer, const vk::Queue &queue) {
		if(*this && buffer && memory && buffer.memory) {
			vk::CommandBufferBeginInfo begininfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
			vk::BufferImageCopy region(
				0, 0, 0,
				vk::ImageSubresourceLayers(
					vk::ImageAspectFlagBits::eColor, 0, 0, 1
				),
				{0, 0, 0},
				extent
			);
			commandbuffer->begin(begininfo);
			commandbuffer->copyBufferToImage(buffer, *this, vk::ImageLayout::eTransferDstOptimal, 1, &region);
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
	void AdvancedImage::transitionLayout(const CommandBuffer &commandbuffer, const vk::Queue &queue, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
		vk::ImageMemoryBarrier barrier(
			{}, {},
			oldLayout, newLayout,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
			*this, vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0, 1, 0, 1
			)
		);
		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if(newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
			if(hasStencilComponent(format)) {
				barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
			}
		}
		else {
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		}
		if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}
		vk::CommandBufferBeginInfo begininfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandbuffer->begin(begininfo);
		commandbuffer->pipelineBarrier(
			sourceStage, destinationStage, {},
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
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