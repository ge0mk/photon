#pragma once

#include <glad/glad.h>
#include <math/vector.hpp>
#include <opengl/renderbuffer.hpp>
#include <opengl/texture.hpp>

#include <unordered_map>
#include <variant>

namespace opengl {
	class FrameBuffer {
	public:
		FrameBuffer(math::ivec2 size);
		FrameBuffer(FrameBuffer &&other);
		~FrameBuffer();

		FrameBuffer& operator=(FrameBuffer &&other);

		void addAttachment(Texture &&texture, GLenum usage);
		void addAttachment(RenderBuffer &&renderbuffer, GLenum usage);

		void attachTexture(const Texture &texture, GLenum usage);
		void attachRenderBuffer(const RenderBuffer &texture, GLenum usage);
		void dettachTexture(GLenum usage);
		void dettachRenderBufffer(GLenum usage);

		void resize(math::ivec2 size);
		math::ivec2 size();

		void bind(GLenum target = GL_FRAMEBUFFER);
		void unbind(GLenum target = GL_FRAMEBUFFER);

		template<typename T>
		T& get(GLenum attachment) {
			return std::get<T>(attachments[attachment]);
		}

		GLenum checkStatus(GLenum target = GL_FRAMEBUFFER);

	private:
		GLuint handle;
		math::ivec2 m_size;
		std::unordered_map<GLenum, std::variant<RenderBuffer, Texture>> attachments;
	};
}