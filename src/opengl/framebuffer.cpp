#include <opengl/framebuffer.hpp>

namespace opengl {
	FrameBuffer::FrameBuffer(math::ivec2 size) : m_size(size) {
		glGenFramebuffers(1, &handle);
	}

	FrameBuffer::FrameBuffer(FrameBuffer &&other) : handle(other.handle), m_size(other.m_size), attachments(std::move(other.attachments)) {
		other.handle = 0;
	}

	FrameBuffer::~FrameBuffer() {
		if(handle) {
			glDeleteFramebuffers(1, &handle);
		}
	}

	FrameBuffer& FrameBuffer::operator=(FrameBuffer &&other) {
		handle = other.handle;
		m_size = other.m_size;
		attachments = std::move(other.attachments);
		other.handle = 0;
		return *this;
	}

	void FrameBuffer::addAttachment(Texture &&texture, GLenum usage) {
		bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, usage, texture.getType(), texture.getHandle(), 0);
		unbind();
		attachments[usage] = std::move(texture);
	}

	void FrameBuffer::addAttachment(RenderBuffer &&renderbuffer, GLenum usage) {
		bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, usage, GL_RENDERBUFFER, renderbuffer.getHandle());
		unbind();
		attachments[usage] = std::move(renderbuffer);
	}

	void FrameBuffer::attachTexture(const Texture &texture, GLenum usage) {
		bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, usage, GL_RENDERBUFFER, texture.getHandle());
		unbind();
	}

	void FrameBuffer::attachRenderBuffer(const RenderBuffer &renderbuffer, GLenum usage) {
		bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, usage, GL_RENDERBUFFER, renderbuffer.getHandle());
		unbind();
	}

	void FrameBuffer::dettachTexture(GLenum usage) {
		bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, usage, GL_RENDERBUFFER, 0);
		unbind();
	}

	void FrameBuffer::dettachRenderBufffer(GLenum usage) {
		bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, usage, GL_RENDERBUFFER, 0);
		unbind();
	}

	void FrameBuffer::resize(math::ivec2 size) {
		for(auto &[usage, attachment] : attachments) {
			switch(attachment.index()) {
				case 0: {
					std::get<RenderBuffer>(attachment).resize(size);
				} break;
				case 1: {
					std::get<Texture>(attachment).resize(size);
				} break;
			}
		}
	}

	math::ivec2 FrameBuffer::size() {
		return m_size;
	}

	void FrameBuffer::bind(GLenum target) {
		glBindFramebuffer(target, handle);
	}

	void FrameBuffer::unbind(GLenum target) {
		glBindFramebuffer(target, 0);
	}

	GLenum FrameBuffer::checkStatus(GLenum target) {
		return glCheckFramebufferStatus(target);
	}
}