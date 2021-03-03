#include <program.hpp>

namespace opengl {
	Program::Program() {
		handle = glCreateProgram();
	}

	Program::Program(Program &&other) : handle(other.handle) {
		other.handle = 0;
	}

	Program::~Program() {
		if(handle) {
			glDeleteProgram(handle);
		}
	}

	Program& Program::operator=(Program &&other) {
		if(handle) {
			glDeleteProgram(handle);
		}
		handle = other.handle;
		other.handle = 0;
		return *this;
	}

	void Program::get(GLenum pname, GLint *params) const {
		glGetProgramiv(handle, pname, params);
	}

	std::string Program::getInfoLog() const {
		if(get<GLint>(GL_LINK_STATUS) != GL_TRUE) {
			GLint len = get<GLint>(GL_INFO_LOG_LENGTH);
			std::string buffer(len, '\0');
			glGetProgramInfoLog(handle, len, &len, buffer.data());
			return buffer;
		}
		return "";
	}

	void Program::attachShader(const Shader &shader) {
		glAttachShader(handle, shader.getHandle());
	}

	void Program::detachShader(const Shader &shader) {
		glDetachShader(handle, shader.getHandle());
	}

	void Program::link() {
		glLinkProgram(handle);
	}

	void Program::use() {
		glUseProgram(handle);
	}

	GLint Program::getUniformLocation(const std::string &name) const {
		return glGetUniformLocation(handle, name.c_str());
	}

	GLint Program::getUniformLocation(const char *name) const {
		return glGetUniformLocation(handle, name);
	}

	void Program::setUniform(GLint location, float value) {
		glUniform1f(handle, value);
	}

	void Program::setUniform(GLint location, math::vec2 value) {
		glUniform2fv(handle, 1, reinterpret_cast<float*>(&value));
	}

	void Program::setUniform(GLint location, math::vec3 value) {
		glUniform3fv(handle, 1, reinterpret_cast<float*>(&value));
	}

	void Program::setUniform(GLint location, math::vec4 value) {
		glUniform4fv(handle, 1, reinterpret_cast<float*>(&value));
	}

	void Program::setUniform(GLint location, int value) {
		glUniform1i(handle, value);
	}

	void Program::setUniform(GLint location, math::ivec2 value) {
		glUniform2iv(handle, 1, reinterpret_cast<int*>(&value));
	}

	void Program::setUniform(GLint location, math::ivec3 value) {
		glUniform3iv(handle, 1, reinterpret_cast<int*>(&value));
	}

	void Program::setUniform(GLint location, math::ivec4 value) {
		glUniform4iv(handle, 1, reinterpret_cast<int*>(&value));
	}

	void Program::setUniform(GLint location, unsigned value) {
		glUniform1ui(handle, value);
	}

	void Program::setUniform(GLint location, math::uvec2 value) {
		glUniform2uiv(handle, 1, reinterpret_cast<unsigned*>(&value));
	}

	void Program::setUniform(GLint location, math::uvec3 value) {
		glUniform3uiv(handle, 1, reinterpret_cast<unsigned*>(&value));
	}

	void Program::setUniform(GLint location, math::uvec4 value) {
		glUniform4uiv(handle, 1, reinterpret_cast<unsigned*>(&value));
	}

	void Program::setUniform(GLint location, math::mat2 value) {
		glUniformMatrix2fv(handle, 1, false, reinterpret_cast<float*>(&value));
	}

	void Program::setUniform(GLint location, math::mat3 value) {
		glUniformMatrix3fv(handle, 1, false, reinterpret_cast<float*>(&value));
	}

	void Program::setUniform(GLint location, math::mat4 value) {
		glUniformMatrix4fv(handle, 1, false, reinterpret_cast<float*>(&value));
	}

	Program Program::load(const std::string &src, uint8_t stages) {
		size_t insertStagePos = src.find("#version");
		insertStagePos = src.find("\n", insertStagePos);
		std::vector<Shader> shaderList;

		if(stages & Shader::VertexStage) {
			shaderList.push_back(std::move(Shader(GL_VERTEX_SHADER)));
			std::string tmp = src;
			tmp.insert(insertStagePos, "\n#define VERTEX_SHADER");
			shaderList.back().setSource(tmp);
			shaderList.back().compile();
			std::string result = shaderList.back().getInfoLog();
			if(result != "") {
				std::cout<<"error in vertex shader:\n"<<result;
			}
		}
		if(stages & Shader::GeometryStage) {
			shaderList.push_back(std::move(Shader(GL_GEOMETRY_SHADER)));
			std::string tmp = src;
			tmp.insert(insertStagePos, "\n#define GEOMETRY_SHADER");
			shaderList.back().setSource(tmp);
			shaderList.back().compile();
			std::string result = shaderList.back().getInfoLog();
			if(result != "") {
				std::cout<<"error in geometry shader:\n"<<result;
			}
		}
		if(stages & Shader::FragmentStage) {
			shaderList.push_back(std::move(Shader(GL_FRAGMENT_SHADER)));
			std::string tmp = src;
			tmp.insert(insertStagePos, "\n#define FRAGMENT_SHADER");
			shaderList.back().setSource(tmp);
			shaderList.back().compile();
			std::string result = shaderList.back().getInfoLog();
			if(result != "") {
				std::cout<<"error in fragment shader:\n"<<result;
			}
		}

		Program prog;
		for(Shader &shader : shaderList) {
			prog.attachShader(shader);
		}
		prog.link();
		std::string result = prog.getInfoLog();
		if(result != "") {
			std::cout<<"error while linking program:\n"<<result;
		}
		for(Shader &shader : shaderList) {
			prog.detachShader(shader);
		}
		return std::move(prog);
	}
}