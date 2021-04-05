#pragma once

#include "buffer.hpp"
#include "vao.hpp"

namespace opengl {
	template<typename ...Components>
	class IndexedMesh {
	public:
		IndexedMesh() : vertexBuffer(opengl::Buffer<Vertex<Components...>>::Array), indexBuffer(opengl::Buffer<unsigned>::ElementArray) {
			vao.bind();
			vertexBuffer.bind();
			indexBuffer.bind();
			vao.setVertexAttributes<Components...>();
			vao.unbind();
		}

		IndexedMesh(const std::vector<Vertex<Components...>> &vertices, const std::vector<unsigned> &indices)
		: vertexBuffer(opengl::Buffer<Vertex<Components...>>::Array), indexBuffer(opengl::Buffer<unsigned>::ElementArray) {
			vao.bind();
			vertexBuffer.bind();
			indexBuffer.bind();
			vao.setVertexAttributes<Components...>();
			vao.unbind();

			setVertexData(vertices);
			setIndexData(indices);
		}

		IndexedMesh(const std::pair<std::vector<Vertex<Components...>>, std::vector<unsigned>> &data)
		: vertexBuffer(opengl::Buffer<Vertex<Components...>>::Array), indexBuffer(opengl::Buffer<unsigned>::ElementArray) {
			vao.bind();
			vertexBuffer.bind();
			indexBuffer.bind();
			vao.setVertexAttributes<Components...>();
			vao.unbind();

			setData(data);
		}

		IndexedMesh(IndexedMesh &&other)
		: vertexBuffer(std::move(other.vertexBuffer)), indexBuffer(std::move(other.indexBuffer)), vao(std::move(other.vao)) {}

		IndexedMesh& operator=(IndexedMesh &&other) {
			vertexBuffer = std::move(other.vertexBuffer);
			indexBuffer = std::move(other.indexBuffer);
			vao = std::move(other.vao);
			return *this;
		}

		void setVertexData(std::vector<Vertex<Components...>> vertices, GLenum usage = opengl::Buffer<Vertex<Components...>>::StaticDraw) {
			vertexBuffer.setData(vertices, usage);
		}

		void setIndexData(std::vector<unsigned> indices, GLenum usage = opengl::Buffer<unsigned>::StaticDraw) {
			indexBuffer.setData(indices, usage);
		}

		void setData(const std::pair<std::vector<Vertex<Components...>>, std::vector<unsigned>> &data) {
			setVertexData(data.first);
			setIndexData(data.second);
		}

		void setData(const std::vector<Vertex<Components...>> &vertices, const std::vector<unsigned> &indces) {
			setVertexData(vertices);
			setIndexData(indces);
		}

		void drawElements(GLenum mode = GL_TRIANGLES, const GLvoid *offset = 0) {
			vao.bind();
			glDrawElements(mode, indexBuffer.size(), GL_UNSIGNED_INT, offset);
			vao.unbind();
		}

	private:
		opengl::Buffer<Vertex<Components...>> vertexBuffer;
		opengl::Buffer<unsigned> indexBuffer;
		opengl::VertexArray vao;
	};


	template<typename ...Components>
	class Mesh {
	public:
		Mesh() : vertexBuffer(opengl::Buffer<Vertex<Components...>>::Array) {
			vao.bind();
			vertexBuffer.bind();
			vao.setVertexAttributes<Components...>();
			vao.unbind();
		}

		Mesh(const std::vector<Vertex<Components...>> &vertices) : vertexBuffer(opengl::Buffer<Vertex<Components...>>::Array) {
			vao.bind();
			vertexBuffer.bind();
			vao.setVertexAttributes<Components...>();
			vao.unbind();

			setVertexData(vertices);
		}

		Mesh(Mesh &&other) : vertexBuffer(std::move(other.vertexBuffer)), vao(std::move(other.vao)) {}

		Mesh& operator=(Mesh &&other) {
			vertexBuffer = std::move(other.vertexBuffer);
			vao = std::move(other.vao);
			return *this;
		}

		void setVertexData(std::vector<Vertex<Components...>> vertices, GLenum usage = opengl::Buffer<Vertex<Components...>>::StaticDraw) {
			vertexBuffer.setData(vertices, usage);
		}

		void setData(std::vector<Vertex<Components...>> &data) {
			setVertexData(data.first);
		}

		void drawElements(GLenum mode = GL_TRIANGLES, GLint offset = 0) {
			vao.bind();
			glDrawArrays(mode, offset, vertexBuffer.size());
			vao.unbind();
		}

	private:
		opengl::Buffer<Vertex<Components...>> vertexBuffer;
		opengl::VertexArray vao;
	};
}