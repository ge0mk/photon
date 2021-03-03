#pragma once

#include <vulkan/vulkan.hpp>
#include <math/vector.hpp>
#include <utils/tuple.hpp>

namespace photonvk {
	template <typename type>
	constexpr vk::Format format() {
		return vk::Format::eUndefined;
	}
	template<> constexpr vk::Format format<math::vec2>() {
		return vk::Format::eR32G32Sfloat;
	}
	template<> constexpr vk::Format format<math::vec3>() {
		return vk::Format::eR32G32B32Sfloat;
	}
	template<> constexpr vk::Format format<math::vec4>() {
		return vk::Format::eR32G32B32A32Sfloat;
	}

	template<typename ...elements>
	class Vertex : public photon::tuple<elements...> {
	public:
		template <typename ...Args>
		Vertex(Args &&... args) : photon::tuple<elements...>(std::forward<Args>(args)...) {}
		static const std::array<vk::VertexInputAttributeDescription, sizeof...(elements)> getAttributeDescriptions(uint32_t binding = 0) {
			std::size_t offset = 0, index = 0;
			return {
				vk::VertexInputAttributeDescription(
					uint32_t(index++), binding, format<elements>(), uint32_t((offset += sizeof(elements)) - sizeof(elements))
				)...
			};
		}
		static const std::array<vk::VertexInputBindingDescription, 1> getBindingDescriptions() {
			return {
				vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex)
			};
		}
	};
}