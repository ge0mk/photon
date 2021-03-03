#include <vao.hpp>

namespace opengl {
	VertexArray::VertexArray() {
		glGenVertexArrays(1, &handle);
	}

	VertexArray::VertexArray(VertexArray &&other) : handle(other.handle) {
		other.handle = 0;
	}

	VertexArray::~VertexArray() {
		if(handle) {
			glDeleteVertexArrays(1, &handle);
		}
	}

	VertexArray& VertexArray::operator=(VertexArray &&other) {
		if(handle) {
			glDeleteVertexArrays(1, &handle);
		}
		handle = other.handle;
		other.handle = 0;
		return *this;
	}

	void VertexArray::bind() {
		glBindVertexArray(handle);
	}

	void VertexArray::unbind() {
		glBindVertexArray(0);
	}

	void VertexArray::enableVertexAttribArray(GLuint index) {
		glEnableVertexAttribArray(index);
	}

	void VertexArray::disableVertexAttribArray(GLuint index) {
		glDisableVertexAttribArray(index);
	}

	void VertexArray::setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
		glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	}

	void VertexArray::setVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
		glVertexAttribIPointer(index, size, type, stride, pointer);
	}
}