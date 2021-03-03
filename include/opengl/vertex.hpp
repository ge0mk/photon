#pragma once

#include <glad/glad.h>

#include <math/vector.hpp>
#include <utils/tuple.hpp>

#include <array>

namespace opengl {
	struct VertexInputAttributeDescription {
		GLuint index;
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLsizei stride;
		const GLvoid *pointer;
	};

	template<typename T>
	constexpr GLenum type() {
		return GL_NONE;
	}
	template<> constexpr GLenum type<float>() { return GL_FLOAT; }
	template<> constexpr GLenum type<math::vec2>() { return GL_FLOAT; }
	template<> constexpr GLenum type<math::vec3>() { return GL_FLOAT; }
	template<> constexpr GLenum type<math::vec4>() { return GL_FLOAT; }
	template<> constexpr GLenum type<int>() { return GL_INT; }
	template<> constexpr GLenum type<math::ivec2>() { return GL_INT; }
	template<> constexpr GLenum type<math::ivec3>() { return GL_INT; }
	template<> constexpr GLenum type<math::ivec4>() { return GL_INT; }
	template<> constexpr GLenum type<unsigned>() { return GL_UNSIGNED_INT; }
	template<> constexpr GLenum type<math::uvec2>() { return GL_UNSIGNED_INT; }
	template<> constexpr GLenum type<math::uvec3>() { return GL_UNSIGNED_INT; }
	template<> constexpr GLenum type<math::uvec4>() { return GL_UNSIGNED_INT; }

	template<typename T>
	constexpr GLint size() {
		return 0;
	}
	template<> constexpr GLint size<float>() { return 1; }
	template<> constexpr GLint size<math::vec2>() { return 2; }
	template<> constexpr GLint size<math::vec3>() { return 3; }
	template<> constexpr GLint size<math::vec4>() { return 4; }
	template<> constexpr GLint size<int>() { return 1; }
	template<> constexpr GLint size<math::ivec2>() { return 2; }
	template<> constexpr GLint size<math::ivec3>() { return 3; }
	template<> constexpr GLint size<math::ivec4>() { return 4; }
	template<> constexpr GLint size<unsigned>() { return 1; }
	template<> constexpr GLint size<math::uvec2>() { return 2; }
	template<> constexpr GLint size<math::uvec3>() { return 3; }
	template<> constexpr GLint size<math::uvec4>() { return 4; }

	template<typename ...Components>
	class Vertex : public photon::tuple<Components...> {
	public:
		Vertex(const Vertex<Components...> &other) : photon::tuple<Components...>(other) {}

		Vertex(Components &&... args) : photon::tuple<Components...>(std::forward<Components>(args)...) {}
		Vertex(const Components &... args) : photon::tuple<Components...>(args...) {}

		operator photon::tuple<Components...>() {
			return *this;
		}

		static const std::array<VertexInputAttributeDescription, sizeof...(Components)> getAttributeDescriptions() {
			std::size_t offset = 0, index = 0;
			GLsizei stride = sizeof(photon::tuple<Components...>);
			return {
				VertexInputAttributeDescription{
					GLuint(index++), size<Components>(), type<Components>(), false, stride, (void*)((offset += sizeof(Components)) - sizeof(Components))
				}...
			};
		}
	};
}

namespace std {
	template <typename ...Components>
	struct tuple_size<opengl::Vertex<Components...>> : tuple_size<photon::tuple<Components...>> {};

	template<size_t N, typename ...Components>
	struct tuple_element<N, opengl::Vertex<Components...>> : tuple_element<N, photon::tuple<Components...>> {};
}
