#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>

namespace opengl {
	class Shader {
	public:
		Shader(GLenum type);
		Shader(Shader &&other);
		~Shader();

		Shader& operator=(Shader &&other);

		void get(GLenum pname, GLint *params);

		template<typename param_t>
		param_t get(GLenum pname) {
			param_t tmp{};
			get(pname, reinterpret_cast<GLint*>(&tmp));
			return tmp;
		}

		std::string getInfoLog();

		void setSource(const std::string &src);
		void setBinary(GLenum format, const std::vector<uint8_t> &data);

		void compile();

		static Shader load(GLenum type, const std::string &src);

		GLuint getHandle() const;

		enum stages {
			VertexStage = 1,
			GeometryStage = 2,
			FragmentStage = 4
		};

	private:
		GLuint handle = 0;
	};

	class VertexShader : public Shader {
		inline VertexShader(Shader &&other) : Shader(std::move(other)) {}
	public:
		inline VertexShader() : Shader(GL_VERTEX_SHADER) {}
		inline static VertexShader load(const std::string &src) {
			return std::move(Shader::load(GL_VERTEX_SHADER, src));
		}
	};

	class FragmentShader : public Shader {
		inline FragmentShader(Shader &&other) : Shader(std::move(other)) {}
	public:
		inline FragmentShader() : Shader(GL_FRAGMENT_SHADER) {}
		inline static FragmentShader load(const std::string &src) {
			return std::move(Shader::load(GL_FRAGMENT_SHADER, src));
		}
	};

	class GeometryShader : public Shader {
		inline GeometryShader(Shader &&other) : Shader(std::move(other)) {}
	public:
		inline GeometryShader() : Shader(GL_GEOMETRY_SHADER) {}
		inline static GeometryShader load(const std::string &src) {
			return std::move(Shader::load(GL_GEOMETRY_SHADER, src));
		}
	};

	class ComputeShader : public Shader {
		inline ComputeShader(Shader &&other) : Shader(std::move(other)) {}
	public:
		inline ComputeShader() : Shader(GL_COMPUTE_SHADER) {}
		inline static ComputeShader load(const std::string &src) {
			return std::move(Shader::load(GL_COMPUTE_SHADER, src));
		}
	};
}