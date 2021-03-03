#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>
#include <math/vector.hpp>
#include <math/matrix.hpp>

#include "shader.hpp"

namespace opengl {
	class Program {
	public:
		Program();
		Program(Program &&other);
		~Program();

		Program& operator=(Program &&other);

		void get(GLenum pname, GLint *params) const;

		template<typename param_t>
		param_t get(GLenum pname) const {
			param_t tmp{};
			get(pname, reinterpret_cast<GLint*>(&tmp));
			return tmp;
		}

		std::string getInfoLog() const;

		void attachShader(const Shader &shader);
		void detachShader(const Shader &shader);
		void link();
		void use();

		GLint getUniformLocation(const std::string &name) const;
		GLint getUniformLocation(const char *name) const;

		void setUniform(GLint location, float value);
		void setUniform(GLint location, math::vec2 value);
		void setUniform(GLint location, math::vec3 value);
		void setUniform(GLint location, math::vec4 value);

		void setUniform(GLint location, int value);
		void setUniform(GLint location, math::ivec2 value);
		void setUniform(GLint location, math::ivec3 value);
		void setUniform(GLint location, math::ivec4 value);

		void setUniform(GLint location, unsigned value);
		void setUniform(GLint location, math::uvec2 value);
		void setUniform(GLint location, math::uvec3 value);
		void setUniform(GLint location, math::uvec4 value);

		void setUniform(GLint location, math::mat2 value);
		void setUniform(GLint location, math::mat3 value);
		void setUniform(GLint location, math::mat4 value);

		template<typename T>
		void setUniform(const std::string &name, const T &value) {
			setUniform(getUniformLocation(name), value);
		}

		static Program load(const std::string &src, uint8_t stages);

	private:
		GLuint handle;
	};
}