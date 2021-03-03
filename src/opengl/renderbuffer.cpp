#include <opengl/renderbuffer.hpp>

namespace opengl {
	RenderBuffer::RenderBuffer() {
		glGenRenderbuffers(1, &handle);
	}

	RenderBuffer::RenderBuffer(GLenum usage, math::ivec2 size) {
		glGenRenderbuffers(1, &handle);
		bind();
		setStorage(usage, size);
		unbind();
	}

	RenderBuffer::RenderBuffer(RenderBuffer &&other) : handle(other.handle), usage(other.usage), m_size(other.m_size) {
		other.handle = 0;
	}

	RenderBuffer::~RenderBuffer() {
		if(handle) {
			glDeleteRenderbuffers(1, &handle);
		}
	}

	RenderBuffer& RenderBuffer::operator=(RenderBuffer &&other) {
		if(handle) {
			glDeleteRenderbuffers(1, &handle);
		}
		usage = other.usage;
		m_size = other.m_size;
		handle = other.handle;
		other.handle = 0;
		return *this;
	}

	GLuint RenderBuffer::getHandle() const {
		return handle;
	}

	GLenum RenderBuffer::getUsage() const {
		return usage;
	}

	math::ivec2 RenderBuffer::size() const {
		return m_size;
	}

	void RenderBuffer::bind(GLenum target) {
		glBindRenderbuffer(target, handle);
	}

	void RenderBuffer::unbind(GLenum target) {
		glBindRenderbuffer(target, 0);
	}

	void RenderBuffer::setStorage(GLenum usage, math::ivec2 size, GLenum target) {
		glRenderbufferStorage(target, usage, size.x, size.y);
		this->usage = usage;
		this->m_size = size;
	}

	void RenderBuffer::resize(math::ivec2 size) {
		bind();
		setStorage(usage, size);
		unbind();
	}
}