#include <opengl/shader.hpp>

namespace opengl {
	Shader::Shader(GLenum type) {
		handle = glCreateShader(type);
	}

	Shader::Shader(Shader &&other) : handle(other.handle) {
		other.handle = 0;
	}

	Shader::~Shader() {
		if(handle) {
			glDeleteShader(handle);
		}
	}

	Shader& Shader::operator=(Shader &&other) {
		if(handle) {
			glDeleteShader(handle);
		}
		handle = other.handle;
		other.handle = 0;
		return *this;
	}

	void Shader::get(GLenum pname, GLint *params) {
		glGetShaderiv(handle, pname, params);
	}

	std::string Shader::getInfoLog() {
		if(get<GLint>(GL_COMPILE_STATUS) != GL_TRUE) {
			GLint len = get<GLint>(GL_INFO_LOG_LENGTH);
			std::string buffer(len, '\0');
			glGetShaderInfoLog(handle, len, &len, buffer.data());
			return buffer;
		}
		return "";
	}


	void Shader::setSource(const std::string &src) {
		std::string buffer = src;
		buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());
		const char *srcptr = buffer.c_str();
		glShaderSource(handle, 1, &srcptr, nullptr);
	}

	void Shader::setBinary(GLenum format, const std::vector<uint8_t> &data) {
		glShaderBinary(1, &handle, format, data.data(), data.size());
	}

	void Shader::compile() {
		glCompileShader(handle);
	}

	Shader Shader::load(GLenum type, const std::string &src) {
		Shader shader(type);
		shader.setSource(src);
		shader.compile();
		return std::move(shader);
	}

	GLuint Shader::getHandle() const {
		return handle;
	}
}