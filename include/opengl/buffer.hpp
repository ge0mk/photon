#pragma once

#include <cstdint>
#include <vector>

#include <glad/glad.h>

namespace opengl {
	template<typename T>
	class Buffer {
	public:
		enum BufferType {
			Array = GL_ARRAY_BUFFER,
			AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
			CopyRead = GL_COPY_READ_BUFFER,
			CopyWrite = GL_COPY_WRITE_BUFFER,
			DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
			DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
			ElementArray = GL_ELEMENT_ARRAY_BUFFER,
			PixelPack = GL_PIXEL_PACK_BUFFER,
			PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
			QUery = GL_QUERY_BUFFER,
			ShaderStorage = GL_SHADER_STORAGE_BUFFER,
			Texture = GL_TEXTURE_BUFFER,
			TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
			Uniform = GL_UNIFORM_BUFFER
		};

		enum BufferUsage {
			StreamDraw = GL_STREAM_DRAW,
			StreamRead = GL_STREAM_READ,
			StreamCopy = GL_STREAM_COPY,
			StaticDraw = GL_STATIC_DRAW,
			StaticRead = GL_STATIC_READ,
			StaticCopy = GL_STATIC_COPY,
			DynamicDraw = GL_DYNAMIC_DRAW,
			DynamicRead = GL_DYNAMIC_READ,
			DynamicCopy = GL_DYNAMIC_COPY
		};

		Buffer(BufferType type) : type(type), m_size(0) {
			glGenBuffers(1, &handle);
		}

		Buffer(const Buffer<T> &other) : type(other.type), m_size(other.m_size) {
			glGenBuffers(1, &handle);

			glBindBuffer(CopyRead, other.handle);
			glBindBuffer(CopyWrite, handle);

			glCopyBufferSubData(CopyRead, CopyWrite, 0, 0, m_size);

			glBindBuffer(CopyRead, 0);
			glBindBuffer(CopyWrite, 0);
		}

		Buffer(Buffer<T> &&other) : type(other.type), m_size(other.m_size), handle(other.handle) {
			other.handle = 0;
		}

		~Buffer() {
			if(handle) {
				glDeleteBuffers(1, &handle);
			}
		}

		Buffer<T>& operator=(const Buffer<T> &other) {
			type = other.type;
			m_size = other.m_size;

			glBindBuffer(CopyRead, other.handle);
			glBindBuffer(CopyWrite, handle);

			glCopyBufferSubData(CopyRead, CopyWrite, 0, 0, m_size);

			glBindBuffer(CopyRead, 0);
			glBindBuffer(CopyWrite, 0);

			return *this;
		}

		Buffer<T>& operator=(Buffer<T> &&other) {
			if(handle) {
				glDeleteBuffers(1, &handle);
			}
			type = other.type;
			m_size = other.m_size;
			handle = other.handle;
			other.handle = 0;

			return *this;
		}

		void bind() {
			glBindBuffer(type, handle);
		}

		void bindBase(GLuint base) {
			glBindBufferBase(type, base, handle);
		}

		void bindRange(GLuint base, GLuint size) {
			glBindBufferRange(type, base, handle, 0, sizeof(T) * size);
		}

		void unbind() {
			glBindBuffer(type, 0);
		}

		void initEmpty(GLenum usage, size_t size = 1) {
			this->m_size = size;

			glBindBuffer(type, handle);
			glBufferData(type, m_size * sizeof(T), nullptr, usage);
			glBindBuffer(type, 0);
		}

		void setData(const std::vector<T> &data, GLenum usage) {
			m_size = data.size();

			glBindBuffer(type, handle);
			glBufferData(type, data.size() * sizeof(T), data.data(), usage);
			glBindBuffer(type, 0);
		}

		void setData(const T &data, GLenum usage) {
			m_size = 1;

			glBindBuffer(type, handle);
			glBufferData(type, sizeof(T), &data, usage);
			glBindBuffer(type, 0);
		}

		void update(const std::vector<T> &data) {
			if(data.size() <= m_size) {
				glBindBuffer(type, handle);
				glBufferSubData(type, 0, m_size * sizeof(T), data.data());
				glBindBuffer(type, 0);
			}
		}

		void update(const T &data) {
			if(m_size >= 1) {
				glBindBuffer(type, handle);
				glBufferSubData(type, 0, sizeof(T), &data);
				glBindBuffer(type, 0);
			}
		}

		std::size_t size() {
			return m_size;
		}

	private:
		GLuint handle;
		BufferType type;
		std::size_t m_size;
	};
}