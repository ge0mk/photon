#pragma once

#include <glad/glad.h>
#include <math/vector.hpp>

namespace opengl {
	class RenderBuffer {
	public:
		RenderBuffer();
		RenderBuffer(GLenum usage, math::ivec2 size);
		RenderBuffer(RenderBuffer &&other);
		~RenderBuffer();

		RenderBuffer& operator=(RenderBuffer &&other);

		GLuint getHandle() const;
		GLenum getUsage() const;
		math::ivec2 size() const;
	
		void bind(GLenum target = GL_RENDERBUFFER);
		void unbind(GLenum target = GL_RENDERBUFFER);
		void setStorage(GLenum usage, math::ivec2 size, GLenum target = GL_RENDERBUFFER);

		void resize(math::ivec2 size);

	private:
		GLuint handle;
		GLenum usage;
		math::ivec2 m_size;
	};
}