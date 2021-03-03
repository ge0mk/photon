#pragma once

#include <glad/glad.h>

#include "vertex.hpp"

namespace opengl {
	class VertexArray {
	public:
		VertexArray();
		VertexArray(VertexArray &&other);
		~VertexArray();

		VertexArray& operator=(VertexArray &&other);

		void bind();
		void unbind();

		void enableVertexAttribArray(GLuint index);
		void disableVertexAttribArray(GLuint index);
		void setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
		void setVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);

		template<typename ...Components>
		void setVertexAttributes() {
			for(const auto &attrib : Vertex<Components...>::getAttributeDescriptions()) {
				enableVertexAttribArray(attrib.index);
				setVertexAttribPointer(attrib.index, attrib.size, attrib.type, attrib.normalized, attrib.stride, attrib.pointer);
			}
		}

	private:
		GLuint handle = 0;
	};
}